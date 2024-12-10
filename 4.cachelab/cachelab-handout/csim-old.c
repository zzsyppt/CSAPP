/**
 * （old version）
 * 10235501419 李佳亮
 */
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define HIT 0
#define MISS 1
#define EVICTION 2

int timeflow = 0;
int showDetails = 0;

// 一个S组，每组E行，每行B字节的数组——用3D Nested Array模拟
typedef struct{
    int B;
    int valid;
    unsigned tag;
    char *cacheblock;
    int recentTimeUsed;
}Cacheline;
void initCacheline(int B, Cacheline *cacheline){
    cacheline->valid = 0;
    cacheline->B = B;
    cacheline->cacheblock = (char *)malloc(B);
    cacheline->recentTimeUsed = 0;
}
void freeCacheline(Cacheline *cacheline){
    free(cacheline->cacheblock);
    cacheline->cacheblock = NULL;
}
void resetCacheline(Cacheline *cacheline){
    cacheline->valid = 0;
    freeCacheline(cacheline);
    cacheline->cacheblock = (char *)malloc(cacheline->B);
}

typedef struct{
    int S, E, B;
    Cacheline **cachelines;
}Cache;
void initCache(int S, int E, int B, Cache *cache){
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->cachelines = (Cacheline **)malloc(sizeof(Cacheline *)*S);
    for(int i=0;i<S;i++){
        cache->cachelines[i] = (Cacheline *)malloc(sizeof(Cacheline)*E);
        // 易错！注意 &cache->cachelines[i][j] 和 cache->cachelines[i]的关系
        for(int j=0;j<E;j++){ 
            initCacheline(B, &cache->cachelines[i][j]);
        }
    }
}
void freeCache(Cache *cache){
    for(int i=0;i<cache->S;i++){
        freeCacheline(cache->cachelines[i]);
        cache->cachelines[i] = NULL;
    }
    free(cache->cachelines);
    cache->cachelines = NULL;
}
int checkCache(unsigned setIndex, unsigned tag, Cache *cache){
    int validCount = 0;
    for(int i=0;i<cache->E;i++){
        Cacheline *cur = &cache->cachelines[setIndex][i];
        if(cur->valid){
            if(cur->tag == tag){
                if(showDetails)
                    printf(" hit");
                /** 易遗漏！hit之后也要修改timeflow! */
                cur->recentTimeUsed = ++timeflow;
                return HIT;
            }
            validCount++;
        }
    }
    if(validCount==cache->E){
        if(showDetails)
            printf(" eviction");
        return EVICTION;
    }
    if(showDetails)
        printf(" miss");
    return MISS;
}
// miss后向cache中加载内容
void loadCache(unsigned tag, unsigned setIndex, unsigned blockOffset, int size, Cache *cache){
    for(int i=0;i<cache->E;i++){
        Cacheline *cur = &cache->cachelines[setIndex][i];
        if(cur->valid == 0){
            cur->valid = 1;
            cur->tag = tag;
            cur->recentTimeUsed = ++timeflow;
            for(int j = blockOffset; j < size + blockOffset && j<cache->B; j++){
                cur->cacheblock[j] = '*';  
            }
            return;
        }
    }
}
// 在cache的setIndex组中查找最近最少使用的cacheline
void LRU_evict(unsigned setIndex, Cache *cache){
    int minTimeUsed = 0x7fffffff; // 易错：应该设置的足够大！0x7fff都不够！
    Cacheline *lru;
    for(int i=0;i<cache->E;i++){
        Cacheline *cur = &cache->cachelines[setIndex][i];
        if(cur->recentTimeUsed < minTimeUsed){
            minTimeUsed = cur->recentTimeUsed;
            lru = cur;
        }
    }
    resetCacheline(lru);
}

void dataProgress(unsigned setIndex, unsigned tag, unsigned blockOffset, int *result, int size, Cache *cache){
    int res = checkCache(setIndex, tag, cache);
    result[res]++;
    if(res == HIT){
        /* DO NOTHING */
    }else if(res == MISS){
        loadCache(tag, setIndex, blockOffset, size, cache);
    }else if(res == EVICTION){
        LRU_evict(setIndex, cache);
        res = checkCache(setIndex, tag, cache);
        result[res]++;
        loadCache(tag, setIndex, blockOffset, size, cache);
    }
}

void printHelpMenu(){
    printf("• -h: Optional help flag that prints usage info \n\
• -v: Optional verbose flag that displays trace info \n\
• -s <s>: Number of set index bits (S = 2s is the number of sets) \n\
• -E <E>: Associativity (number of lines per set) \n\
• -b <b>: Number of block bits (B = 2b is the block size) \n\
• -t <tracefile>: Name of the valgrind trace to replay \n");
}

int main(int argc,char* argv[])
{
    // 输出数据
    int result[3] = {0};
    // 处理命令行参数
    int s, S, E, b, B, t, C;
    char *tracefile;
    const char *optstr = "hvs:E:b:t:";
    int opt;
    while((opt = getopt(argc, argv, optstr)) != -1){
        switch(opt){
            case 'h':
                printHelpMenu();
                return 0;
            case 'v':
                showDetails = 1;
                break;
            case 's':
                s = atoi(optarg);   // 外部变量 optarg 指向当前选项参数的指针
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
        }
    }
    
    S = 1<<s;   // 用左移表示幂运算
    B = 1<<b;
    t = 64-b-s;
    C = B*E*S;

    // 初始化cache
    Cache cache;
    initCache(S, E, B, &cache);
    
    // 读取数据文件
    FILE *fp = fopen(tracefile, "r");
    char buf[256];
    char type;
    char *addr, *sz;
    unsigned address, tag, setIndex, blockOffset;
    int size;

    while(fgets(buf, 255, fp)){
        // 不进行"I"指令
        if(buf[0] != ' '){             
            continue;
        }
        if(buf[strlen(buf)-1] == '\n'){
            buf[strlen(buf)-1]  = '\0';
        }
        if(showDetails)
            printf("\n%s", buf);
        // 分解行指令
        type = buf[1];
        /* 易错！ address is hex! */
        addr = strtok(buf+3, ",");
        char *endptr;
        address = strtol(addr, &endptr, 16);
        // 每次找到一个分隔符后，一个NULL就被放到分隔符处，函数用这种方法来连续查找该字符串。
        sz = strtok(NULL, ",");
        size = atoi(sz);
        // 拆解地址：t位tag位，s位set index，b位block offset
        tag = address >> s+b;
        setIndex = (address >> b) & ((1<<s)-1);  //(1<<s)-1=2^s-1=0b11..1（s个1）
        blockOffset = address & ((1<<b)-1);
        int res;
        switch(type){
            // L for data load
            case 'L':
                dataProgress(setIndex, tag, blockOffset, result, size, &cache);
                break;
            // S for data store
            case 'S':
                dataProgress(setIndex, tag, blockOffset, result, size, &cache);
                break;
            // M for data modify
            case 'M':
                dataProgress(setIndex, tag, blockOffset, result, size, &cache);
                dataProgress(setIndex, tag, blockOffset, result, size, &cache);
                break;
        }
    }
    if(showDetails)
        printf("\n");
    printSummary(result[HIT], result[MISS], result[EVICTION]);
    freeCache(&cache);
    return 0;
}
/**
 * 遇到的问题：
 * 如何管理可变个命令行参数/如何处理可选的参数
 * 这三个指令有什么区别？
 * 选择何种数据结构实现cache
 * 是否cache需要完整的实现
 * 
 * 陷入误区：miss后不会再hit
 * 
 * 待改进的地方：
 * 1.数据结构（如链表LRU）
 * 2.HIT MISS EVICTION处理中“包含”的部分（case语句）
 * 3.命令行参数 以及Linux内置的写好的轮子
 * 4.fgets()重复读取最后一行的分析及解决方法
 * 
 * zzsyp@Mark-PC2:~/csapp/cachelab/cachelab-handout$ ./csim -s 2 -E 2 -b 3 -t traces/trans.trace -v

 S 00600aa0,1 miss
 S 7ff000398,8 miss
 S 7ff000390,8 miss
Segmentation fault
*/