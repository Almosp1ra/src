#include <stdio.h>
#include <sys.h>

int main1(int argc, char* argv[])
{
    int i, j;
    if(fork())
    {
        printf("father.\n");
        if(fork())
        {
            i = wait(&j);
            printf("exit child = %d, exit status = %d.\n", i, j >> 8);
        }
        else
        {
            printf("second child.\n");
            exit(3);
        }
    }
    else
    {
        sleep(2);
        printf("first child.\n");
        exit(5);
    }
}
