#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int atoi(const char* s, int* isValid)
{
    int n = 0, sign = 1;

    if (*s == '-')
    {
        sign = -1;
        s++;
    }

    while (*s >= '0' && *s <= '9')
    {
        n = n * 10 + (*s - '0');
        s++;
    }

    *isValid = (*s == 0 ? 1 : 0);

    return sign * n;
}

int
main(void)
{
    int a, b;
    int c;
    int isValid;
    char inputBuffer[512];

    while(1)
    {
        print("Input dividend: \n");
        gets(inputBuffer);
        a = atoi(inputBuffer, &isValid);

        if(isValid == 1)
        {
            printf("Invalid input.\n");
            continue;
        }

        print("Input divisor: \n");
        gets(inputBuffer);
        b = atoi(inputBuffer, &isValid);

        if(isValid == 1)
        {
            printf("Invalid input.\n");
            continue;
        }
        else if(b == 0)
        {
            printf("Divisor can not be 0.\n");
            continue;
        }

        c = a / b;
        printf("%d / %d = %d\n", a, b, c);
    }
}
