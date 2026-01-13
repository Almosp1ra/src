/*
 * trivialProg.c
 *
 *  Created on: 2024-11-3
 *      Author: Deng Rong
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

int main1()
{
    int pid;
    int exitCode;
    
    char* argv[]={"trivialProg", "arg1", "arg2", "arg3"};

    pid = fork();
    if(pid == 0) {
        execv(argv[0], argv);
        printf("Done!\n");
    }
    else
        wait(&exitCode);
    printf("The end of tryExec.\n");
}

