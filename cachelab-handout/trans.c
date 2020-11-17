/* 
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
    //notice that this probles A is N * M not M * N
    //this lab need you to use the block-divide method to reduce the miss
    int a0,a1,a2,a3,a4,a5,a6,a7;
    if(M == 32){
	for(int i = 0;i < N;i += 8){
	    for(int j = 0;j < M;j += 8){		
		for(int k = i;k < i + 8 && k < M;k++){
		    a0 = A[k][j];
		    a1 = A[k][j + 1];
		    a2 = A[k][j + 2];
		    a3 = A[k][j + 3];
		    a4 = A[k][j + 4];
		    a5 = A[k][j + 5];
		    a6 = A[k][j + 6];
		    a7 = A[k][j + 7];

		    B[j][k] = a0;
		    B[j + 1][k] = a1;
		    B[j + 2][k] = a2;
		    B[j + 3][k] = a3;
		    B[j + 4][k] = a4;
		    B[j + 5][k] = a5;
		    B[j + 6][k] = a6;
		    B[j + 7][k] = a7;
		}
	    }
	}
    }else if(M == 64){
	for(int i = 0;i < N;i += 8)
	    for(int j = 0;j < M;j += 8){
	        for(int k = i;k < i + 4;++k){
		    a0 = A[k][j];
		    a1 = A[k][j + 1];
		    a2 = A[k][j + 2];
		    a3 = A[k][j + 3];
		    a4 = A[k][j + 4];
		    a5 = A[k][j + 5];
		    a6 = A[k][j + 6];
		    a7 = A[k][j + 7];
		
		    B[j][k] = a0;
		    B[j + 1][k] = a1;
		    B[j + 2][k] = a2;
		    B[j + 3][k] = a3;
		    B[j][k + 4] = a7; 
		    B[j + 1][k + 4] = a6;
		    B[j + 2][k + 4] = a5;
		    B[j + 3][k + 4] = a4;
		}
		
		for(int l = 0;l < 4;++l){
		    a0 = A[i + 4][j + 3 - l];
		    a1 = A[i + 5][j + 3 - l];
		    a2 = A[i + 6][j + 3 - l];
		    a3 = A[i + 7][j + 3 - l];
		    a4 = A[i + 4][j + l + 4];
		    a5 = A[i + 5][j + l + 4];
		    a6 = A[i + 6][j + l + 4];
		    a7 = A[i + 7][j + l + 4];
		
		    B[j + 4 + l][i] = B[j + 3 -l][i + 4];
		    B[j + 4 + l][i + 1] = B[j + 3 - l][i + 5];
		    B[j + 4 + l][i + 2] = B[j + 3 - l][i + 6];
		    B[j + 4 + l][i + 3] = B[j + 3 - l][i + 7];

		    B[j + 3 - l][i + 4] = a0;
		    B[j + 3 - l][i + 5] = a1;
		    B[j + 3 - l][i + 6] = a2;
		    B[j + 3 - l][i + 7] = a3;
		    B[j + 4 + l][i + 4] = a4;
		    B[j + 4 + l][i + 5] = a5;
		    B[j + 4 + l][i + 6] = a6;
		    B[j + 4 + l][i + 7] = a7;
		}
        }
    }else if(M == 61){
	for(int i = 0;i < N;i += 17)
	    for(int j = 0;j < M;j += 17)
		for(int k = i;k < i + 17 && k < N;++k)
		    for(int l = j;l < j + 17 && l < M;++l)
			B[l][k] = A[k][l];
    }
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

