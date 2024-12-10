/**
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
    int valid;
    unsigned tag;
    int recentTimeUsed;
}Cacheline;
void initCacheline(Cacheline *cacheline){
    cacheline->valid = 0;
    cacheline->tag = 0;
    cacheline->recentTimeUsed = 0;
}

typedef struct{
    int S, E;
    Cacheline **cachelines;
}Cache;
void initCache(int S, int E, Cache *cache){
    cache->S = S;
    cache->E = E; 
    cache->cachelines = (Cacheline **)malloc(sizeof(Cacheline *)*S);
    for(int i=0;i<S;i++){
        cache->cachelines[i] = (Cacheline *)malloc(sizeof(Cacheline)*E);
        // 易错！注意 &cache->cachelines[i][j] 和 cache->cachelines[i]的关系
        for(int j=0;j<E;j++){ 
            initCacheline(&cache->cachelines[i][j]);
        }
    }
}
void freeCache(Cache *cache){
    for(int i=0;i<cache->S;i++){
        free(cache->cachelines[i]);
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
void loadCache(unsigned tag, unsigned setIndex, Cache *cache){
    for(int i=0;i<cache->E;i++){
        Cacheline *cur = &cache->cachelines[setIndex][i];
        if(cur->valid == 0){
            cur->valid = 1;
            cur->tag = tag;
            cur->recentTimeUsed = ++timeflow; 
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
    initCacheline(lru);
}

// 模拟Data Load/Store操作
void simulate(unsigned setIndex, unsigned tag, int *result, Cache *cache){
    int res = checkCache(setIndex, tag, cache);
    result[res]++;
    // 如果HIT，无需再进行操作
    // 如果MISS，向cache加载相应数据
    if(res == MISS){
        loadCache(tag, setIndex, cache);
    }
    // 如果EVICTION，驱逐后再次访问缓存不命中，之后向cache加载相应数据
    else if(res == EVICTION){
        LRU_evict(setIndex, cache);
        res = checkCache(setIndex, tag, cache);
        result[res]++;
        /* 事实上eviction后的miss是必然的，故这两行可直接改为result[MISS]++ */
        loadCache(tag, setIndex, cache);
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

// 读取命令行参数
void readCommandline(int argc, char* argv[], int *s, int *E, int *b, char *tracefile){
    const char *optstr = "hvs:E:b:t:";
    int opt;
    while((opt = getopt(argc, argv, optstr)) != -1){
        switch(opt){
            case 'h':
                printHelpMenu();	// 打印帮助菜单
                exit(0);
            case 'v':
                showDetails = 1;	// 设置全局变量showDetails为真
                break;
            case 's':
                *s = atoi(optarg);   // 外部变量 optarg - 指向当前选项参数字符串的指针
                break;
            case 'E':
                *E = atoi(optarg);
                break;
            case 'b':
                *b = atoi(optarg);
                break;
            case 't':
                strncpy(tracefile, optarg, strlen(optarg));
                break;
        }
    }
}

// result：记录总的hit, miss, eviction数的数组
void doCacheSimulation(char *tracefile, int s, int b, Cache *cache, int result[3]){
    // 读取数据文件
    FILE *fp = fopen(tracefile, "r");
    char buf[256];
    char type;
    char *addr; // 存储address的字符串临时变量
    unsigned address, tag, setIndex;

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
        // 拆解地址：t位tag位，s位set index，b位block offset
        tag = address >> (s+b); 
        setIndex = (address >> b) & ((1<<s)-1);  //(1<<s)-1=2^s-1=0b11..1（s个1）
 
        switch(type){
            // L for data load, S for data store
            case 'L':
            case 'S':
                simulate(setIndex, tag, result, cache);
                break;
            // M for data modify
            case 'M':
                simulate(setIndex, tag, result, cache);
                simulate(setIndex, tag, result, cache);
                break;
        }
    }
}

int main(int argc,char* argv[])
{
    // 处理命令行参数
    int s, S, E, b;
    char tracefile[128];
    readCommandline(argc, argv, &s, &E, &b, tracefile);
    S = 1<<s;   // 用左移表示幂运算

    // 初始化cache
    Cache cache;
    initCache(S, E, &cache);
    
    // 模拟缓存过程
    int result[3] = {0};
    doCacheSimulation(tracefile, s, b , &cache, result);


    // 输出结果
    if(showDetails)
        printf("\n");
    printSummary(result[HIT], result[MISS], result[EVICTION]);
    
    // 回收内存
    freeCache(&cache);

    return 0;
}
