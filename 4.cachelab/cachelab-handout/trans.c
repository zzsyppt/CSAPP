/* 
 * 10235501419 李佳亮 
 *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void trans_32_32(int A[32][32], int B[32][32]){
    int ii, jj, i, t1, t2, t3, t4, t5, t6, t7, t8;
    // ii, jj在外层遍历矩阵，指向每个分块矩阵的第一行/列
    for(ii = 0; ii < 32; ii += 8){
        for(jj = 0; jj < 32; jj += 8){
            // i遍历子矩阵，指向子矩阵的某一行/列在原矩阵中的行/列
            for(i = ii; i < 8 + ii; i++){
                t1 = A[i][jj];
                t2 = A[i][jj+1];
                t3 = A[i][jj+2];
                t4 = A[i][jj+3];
                t5 = A[i][jj+4];
                t6 = A[i][jj+5];
                t7 = A[i][jj+6];
                t8 = A[i][jj+7];
                B[jj][i] = t1;
                B[jj+1][i] = t2;
                B[jj+2][i] = t3;
                B[jj+3][i] = t4;
                B[jj+4][i] = t5;
                B[jj+5][i] = t6;
                B[jj+6][i] = t7;
                B[jj+7][i] = t8;
            }
        }
    }
}

void trans_64_64(int A[64][64], int B[64][64]){
    int ii, jj, i, j, t1, t2, t3, t4, t5, t6, t7, t8;
    // ii, jj在外层遍历矩阵，指向每个分块矩阵的第一行/列
    for(ii = 0; ii < 64; ii += 8){
        for(jj = 0; jj < 64; jj += 8){ 
            // i, j遍历子矩阵，指向子矩阵的某一行/列在原矩阵中的行/列
            // (1)
            // 这里采用临时变量而不直接循环，是因为优化对角线元素
            for(i = ii; i < 4 + ii; i++){
                t1 = A[i][jj];
                t2 = A[i][jj+1];
                t3 = A[i][jj+2];
                t4 = A[i][jj+3];
                t5 = A[i][jj+4];
                t6 = A[i][jj+5];
                t7 = A[i][jj+6];
                t8 = A[i][jj+7];
                B[jj][i] = t1;
                B[jj+1][i] = t2;
                B[jj+2][i] = t3;
                B[jj+3][i] = t4;
                B[jj][i+4] = t5;
                B[jj+1][i+4] = t6;
                B[jj+2][i+4] = t7;
                B[jj+3][i+4] = t8;
            }
            // (2)
            for(j = jj; j < 4 + jj; j++){
                // i.暂存错误的元素
                t1 = B[j][ii+4];
                t2 = B[j][ii+5];
                t3 = B[j][ii+6];
                t4 = B[j][ii+7]; 
                // ii.把粉色部分放进正确的位置
                B[j][ii+4] = A[ii+4][j];
                B[j][ii+5] = A[ii+5][j];
                B[j][ii+6] = A[ii+6][j];
                B[j][ii+7] = A[ii+7][j];
                // iii.把错误的元素放进正确的位置
                B[j+4][ii] = t1;
                B[j+4][ii+1] = t2;
                B[j+4][ii+2] = t3;
                B[j+4][ii+3] = t4;
            }
            // (3)
            // 这里采用临时变量而不直接循环，是因为优化对角线元素
            for(int i = ii + 4; i < ii + 8; i++){
                t1 = A[i][jj+4];
                t2 = A[i][jj+5];
                t3 = A[i][jj+6];
                t4 = A[i][jj+7];
                B[jj+4][i] = t1;
                B[jj+5][i] = t2;
                B[jj+6][i] = t3;
                B[jj+7][i] = t4;
            }
        }
    }
}

void trans_61_67(int A[67][61], int B[61][67]){ 
    // ii, jj在外层遍历矩阵，指向每个分块矩阵的第一行/列
    for(int ii = 0; ii < 67; ii += 16){
        for(int jj = 0; jj < 61; jj += 16){
            // i,j 遍历子矩阵，指向子矩阵的某一行/列在原矩阵中的行/列
            for(int i = ii; i < 16 + ii && i < 67; i++){
                for(int j = jj; j < 16 + jj && j < 61; j++){
                    B[j][i] = A[i][j];
                }
            }
        }
    } 
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M == 32 && N == 32)
        trans_32_32(A, B);
    else if(M == 64 && N == 64)
        trans_64_64(A, B);
    else if(M == 61 && N == 67)
        trans_61_67(A, B);

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */  

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


/**
 * 
 *安装 valgrind
 教程里说：可以写多个函数，只需要在register注册。

Function 1就是
Function 1 (2 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 1 (Simple row-wise scan transpose): hits:869, misses:1184, evictions:1152

确定A,B首地址的间距

防迷思：局部变量存在CPU的寄存器里

由于是限制了12个local variable，所以要用`hardcode `
*/
