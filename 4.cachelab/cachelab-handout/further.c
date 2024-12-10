#include<stdio.h>
#include<stdlib.h>
int main(int argc, char *argv[])
{	
	char *endptr;
    unsigned a_firstAddr = strtol(argv[1], &endptr, 16);
	int count=0;
	for(int x = 0; x < 1024; x++)
	{
		unsigned int y = a_firstAddr + 4 * x;
		if( (y >> 5U) % 32U == (a_firstAddr >> 5U) % 32U){ //和A一个缓存块
			int x_ = x/32;
			int y_ = x%32;
			printf("M[%d][%d], ", x_,y_);
			count++;
			if(count%8 == 0)
				printf("\n");
		}
	}
	return 0;
}