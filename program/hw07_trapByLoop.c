#include <stdio.h>
#include <sys.h>

static void sig_dyzero(int signo)
{
    printf("Divide by zero!\n");
}

int main1(void)
{
    int a, b;
    int c;

    if(signal(SIGFPE, sig_dyzero) < 0)
    {
        printf("can't catch divide by zero error!");
    }

    for(;;)
    {
        printf("请输入被除数\n");
        scanf("%d", &a);

        printf("请输入除数\n");
        scanf("%d", &b);

        c = a / b;
        printf("%d / %d = %d\n", a, b, c);
    }
}
