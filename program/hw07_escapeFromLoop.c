#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static sigjmp_buf fpe_env;

static void sig_dyzero(int signo)
{
    printf("Divide by zero!\n");
}

int
main(void)
{
    int a, b;
    int c;

    if(signal(SIGFPE, sig_dyzero) == SIG_ERR)
    {
        printf("can't catch divide by zero error!");
        siglongjmp(fpe_env, 0);
    }

    for(;;)
    {
        sigsetjmp(fpe_env, 1);

        print("请输入被除数\n");
        scanf("%d", &a);

        print("请输入除数\n");
        scanf("%d", &b);

        c = a / b;
        printf("%d / %d = %d\n", a, b, c);
    }
}
