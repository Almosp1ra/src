#include <stdio.h>
#include <stdlib.h>

#define M 32
int version = 1;
int uninit; // bss segment
int matrixOriginalA[M][M];
int matrixOriginalB[M][M];
int matrixDes[M][M];

void produce(int row, int column) {
    int i;
    for (i = 0; i < M; i++)
        matrixDes[row][column] += matrixOriginalA[row][i] * matrixOriginalB[column][i];
}

static int my_atoi(const char *s) {
    int sign = 1;
    int v = 0;
    if (!s) return 0;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '+') s++;
    else if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }
    return sign * v;
}
   
int main1(int argc, char *argv[]) {
    int i, j;
    for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++) {
            matrixOriginalA[i][j] = 1;
            matrixOriginalB[j][i] = 1;
            matrixDes[i][j] = 0;
        }
    }

    // 设置进程数量
    int num_procs = 2;
    if (argc >= 2) {
        int v = my_atoi(argv[1]);
        if (v > 0 && v <= 16)
            num_procs = v;
    }

    int rank = 0;       // 当前进程的编号，0表示父进程
    int pid;
    int child_pids[16]; // 父进程保存子 pid，最多 16

    /* --- 创建 (num_procs - 1) 个子进程 --- */
    for (i = 1; i < num_procs; i++) {
        pid = fork();
        if (pid < 0) {
            break;
        }
        if (pid == 0) {
            rank = i;
            break;
        }
        else {
            child_pids[i] = pid;
        }
    }

    /* 每个进程计算自己那一段行：[from, to) */
    int from = (rank * M) / num_procs;
    int to   = ((rank + 1) * M) / num_procs;
    for (i = from; i < to; i++) {
        for (j = 0; j < M; j++) {
            produce(i, j);
        }
    }

    /* 如果是父进程（rank == 0），等待所有子进程结束 */
    if (rank == 0) {
        int status;
        int waited = 0;
        while (waited < (num_procs - 1)) {
            int w = wait(&status);
            if (w > 0) waited++;
            else break;
        }
    }

    // 输出
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
        printf("calculation complete, rank = %d\n, pid = ", rank, getpid());
    }

    return 0;
}
