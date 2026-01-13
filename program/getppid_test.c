#include <stdio.h>
#include <stdlib.h>

int main1(int argc, char* argv[])
{
	int ppid = getppid();
	
	printf("My PPID is %d\n",ppid);

	exit(0);
}
