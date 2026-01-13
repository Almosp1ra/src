#include <stdio.h>
#include <stdlib.h>
#include <sys.h>

int main1(int argc, char* argv[])
{
	struct proc_info proc_info;

	int res = getproc(&proc_info);
	
	// 虚拟地址
	printf("x_daddr = %d\n",proc_info.x_daddr);
	printf("x_caddr = %d\n",proc_info.x_caddr);
	printf("x_size  = %d\n",proc_info.x_size);
	printf("p_addr  = %d\n",proc_info.p_addr);
	printf("p_size  = %d\n",proc_info.p_size);
	// 物理地址
	printf("m_TextStartAddress = %d\n",proc_info.m_TextStartAddress);
	printf("m_TextSize		   = %d\n",proc_info.m_TextSize);
	printf("m_DataStartAddress = %d\n",proc_info.m_DataStartAddress);
	printf("m_DataSize		   = %d\n",proc_info.m_DataSize);
	printf("m_StackSize		   = %d\n",proc_info.m_StackSize);

	exit(0);
}
