#include <stdio.h>
#include <sys.h>

int main1(int argc, char* argv[])
{
    int pid;
    
    if (!(pid = fork()))    // 子进程，无限循环
    {
        printf("child start\n");
        while (1) {
            sleep(2);
            printf("child is running, pid = %d\n", getpid());
        }
        printf("child end\n");
    }
    else    // 父进程，子进程运行一段时间后杀死子进程
    {
        printf("parent start\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        sleep(9);

        printf("parent kills child\n");
        kill(pid, SIGKILL);

        printf("parent end\n");
    }

    return 0;
}
