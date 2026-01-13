#include <stdio.h>
#include <stdlib.h>

#define M 512
int version = 1;
int matrixOriginal[M][M];
int matrixDes[M][M];

void produce(int row, int column) {
    int i;
    for (i = 0; i < M; i++)
        matrixDes[row][column] += matrixOriginal[row][i] * matrixOriginal[i][column];
}

   
int main1() {
    int i, j;
    for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++) {
            matrixOriginal[i][j] = 1;
            matrixDes[i][j] = 0;
        }
    }

    int child;
    int from, to;
    child = fork();
    if (child > 0) {    // 父进程算后半 M/2 行
        from = M / 2;
        to = M;
        for (i = from; i < to; i++)
            for (j = 0; j < M; j++)
                produce(i, j);
        wait(); // 等待子进程输出前半部分
    }
    else {    // 子进程算前半 M/2 行
        from = 0;
        to = M / 2;
        for (i = from; i < to; i++)
            for (j = 0; j < M; j++)
                produce(i, j);
    }

    // 各自输出
    int PRINT_RESULTS = 0;
    if (PRINT_RESULTS) {
        for (i = from; i < to; i++) {
            for (j = 0; j < M; j++) {
                printf("%d\t", matrixDes[i][j]);
            }
            printf("\n");
        }
    }
    else {
        printf("calculation complete\n");
    }
   
    return 0;
}
