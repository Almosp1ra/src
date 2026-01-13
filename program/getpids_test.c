#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

int main1(int argc, char* argv[])
{
	struct pids pids;

	int res = getpids(&pids);
	
	printf("PID  = %d\n",pids.pid);
	printf("PPID = %d\n",pids.ppid);

	exit(0);
}
