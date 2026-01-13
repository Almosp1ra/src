#include <stdio.h>
#include <sys.h>

void WhenReceiveCtrlC()
{
    printf("You're early! This process of Unix V6++ is preparing for release. Please check back in a few minutes.\n");
}

int main1(int argc, char* argv[])
{
    signal(SIGINT, WhenReceiveCtrlC);

    printf("Process start.\n");

    while(1)
    {
        signal(SIGINT, WhenReceiveCtrlC);
        printf("Still running...\n");
        sleep(10);
    }

    printf("Process end.\n");

    return 0;
}
