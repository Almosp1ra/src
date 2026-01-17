#include "MemoryDescriptor.h"
#include "Kernel.h"
#include "PageManager.h"
#include "Machine.h"
#include "PageDirectory.h"
#include "Video.h"

/* 不使用相对表，以下函数实际上不需要了 */

void MemoryDescriptor::Initialize()
{
}

void MemoryDescriptor::Release()
{
}

unsigned int MemoryDescriptor::MapEntry(unsigned long virtualAddress, unsigned int size, unsigned long phyPageIdx, bool isReadWrite)
{	
}

void MemoryDescriptor::MapTextEntrys(unsigned long textStartAddress, unsigned long textSize, unsigned long textPageIdx)
{
}
void MemoryDescriptor::MapDataEntrys(unsigned long dataStartAddress, unsigned long dataSize, unsigned long dataPageIdx)
{
}

void MemoryDescriptor::MapStackEntrys(unsigned long stackSize, unsigned long stackPageIdx)
{
}

/* GetUserPageTableArray不需要 */

PageTable* MemoryDescriptor::GetUserPageTableArray()
{
}
unsigned long MemoryDescriptor::GetTextStartAddress()
{
	return this->m_TextStartAddress;
}
unsigned long MemoryDescriptor::GetTextSize()
{
	return this->m_TextSize;
}
unsigned long MemoryDescriptor::GetDataStartAddress()
{
	return this->m_DataStartAddress;
}
unsigned long MemoryDescriptor::GetDataSize()
{
	return this->m_DataSize;
}
unsigned long MemoryDescriptor::GetStackSize()
{
	return this->m_StackSize;
}

/* 检查是否超出空间限制，然后直接调用 MapToPageTable 建立页表 */

bool MemoryDescriptor::EstablishUserPageTable( unsigned long textVirtualAddress, unsigned long textSize, unsigned long dataVirtualAddress, unsigned long dataSize, unsigned long stackSize, unsigned long rdataVirtualAddress, unsigned long rdataSize )
{
	User& u = Kernel::Instance().GetUser();

	/* 如果超出允许的用户程序最大8M的地址空间限制 */
	if ( textSize + dataSize + stackSize  + PageManager::PAGE_SIZE > USER_SPACE_SIZE - textVirtualAddress)
	{
		u.u_error = User::ENOMEM;
		Diagnose::Write("u.u_error = %d\n",u.u_error);
		return false;
	}

	/* 给的实验参考里，这里更新了 MemoryDescriptor 的各逻辑段信息，但是按现在的实现来看不做也可以
	 * 因为目前引用这个函数的场景，输入参数都是直接用的 MemoryDescriptor 的各逻辑段信息，所以相当于没更新
	 * 除了 Exec 中输入参数是程序头里的逻辑段的信息，不过Exec里同样包含用这些信息更新 MD 字段的逻辑，所以用不着
	 */

	this->m_TextStartAddress = textVirtualAddress;
	this->m_TextSize = textSize;
	this->m_DataStartAddress = dataVirtualAddress;
	this->m_DataSize = dataSize;
	this->m_StackSize = stackSize;
	this->m_RdataStartAddress = rdataVirtualAddress;
	this->m_RdataSize = rdataSize;

	/* 将相对地址映照表根据正文段和数据段在内存中的起始地址pText->x_caddr、p_addr，建立用户态内存区的页表映射 */
	this->MapToPageTable();
	return true;
}

/* ClearUserPageTable 不需要 */

void MemoryDescriptor::ClearUserPageTable()
{
}

/* DisplayPageTable 注释掉显示相对表的逻辑，只显示页表（不过系统打印区显示不了所以也没什么用） */

void MemoryDescriptor::DisplayPageTable()
{
	unsigned int i,j;

	Diagnose::Write("The length of MemoryDescriptor is %d Byte.\n",sizeof(MemoryDescriptor));

	Diagnose::Write("<PPDA,%x>  ",Machine::Instance().GetKernelPageTable().m_Entrys[1023].m_PageBaseAddress);

	PageTable* pUserPageTable = Machine::Instance().GetUserPageTableArray();
	Diagnose::Write("System PT:");
	for (i = 0; i < Machine::USER_PAGE_TABLE_CNT; i++)
		for ( j = 0; j < PageTable::ENTRY_CNT_PER_PAGETABLE; j++)
			if ( 1 == pUserPageTable[i].m_Entrys[j].m_Present )
				Diagnose::Write("<%d,%x>  ",i*1024+j,pUserPageTable[i].m_Entrys[j].m_PageBaseAddress);
	Diagnose::Write("\n");
}

/* 去除相对表，主要对 MapToPageTbale 进行修改。
 * 主要思想：MemoryDescriptor 里已经有了代码段、数据段的起始地址和长度字段，那么就不需要相对表，
 * 直接用这些字段进行映射即可。
 * （顺便一提，设置 MemoryDescriptor 里这些字段的是 exec 系统调用）
 */

void MemoryDescriptor::MapToPageTable()
{
	User& u = Kernel::Instance().GetUser();

	PageTable* pUserPageTable = Machine::Instance().GetUserPageTableArray();

	/* 得到共享正文段和可交换部分的物理页框号 */

	unsigned int textPF = 0;
	if ( u.u_procp->p_textp != NULL )
	{
		textPF = u.u_procp->p_textp->x_caddr >> 12;
	}

	unsigned int pAddrPF = u.u_procp->p_addr >> 12;

	/* 共享正文段的映射信息 */

	unsigned long textStartUserPageTabelIdx = this->m_TextStartAddress >> 22;
	unsigned long textStartEntryIdx = (this->m_TextStartAddress >> 12) & 0x3ff;
	unsigned long textEntryCnt = (this->m_TextSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;
	unsigned long textIdx = 0;

	/* 可交换部分的映射信息 */

	unsigned long dataStartUserPageTabelIdx = this->m_DataStartAddress >> 22;
	unsigned long dataStartEntryIdx = (this->m_DataStartAddress >> 12) & 0x3ff;
	unsigned long dataEntryCnt = (this->m_DataSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;
	unsigned long dataIdx = 1;

	/* 堆栈段的映射信息，栈固定在用户第二张页表末尾 */

	unsigned long stackStartAddress = (USER_SPACE_START_ADDRESS + USER_SPACE_SIZE - this->m_StackSize) & 0xFFFFF000;
	unsigned long stackStartUserPageTabelIdx = stackStartAddress >> 22;	// 实际固定为 1
	unsigned long stackStartEntryIdx = (stackStartAddress >> 12) & 0x3ff;
	unsigned long stackEntryCnt = (this->m_StackSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;

	/* 只读数据段的映射信息，虚空间中属于数据段，实空间中和代码段放在一起 */

	unsigned long rdataStartUserPageTabelIdx = dataStartUserPageTabelIdx;
	unsigned long rdataStartEntryIdx = (this->m_RdataStartAddress >> 12) & 0x3ff;
	unsigned long rdataEntryCnt = (this->m_RdataSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;

	/* 映射 */

	for (unsigned int i = 0; i < Machine::USER_PAGE_TABLE_CNT; i++)
	{
		for ( unsigned int j = 0; j < PageTable::ENTRY_CNT_PER_PAGETABLE; j++ )
		{
			pUserPageTable[i].m_Entrys[j].m_Present = 0;   // 清0表示该逻辑页不存在
			
			if (i == textStartUserPageTabelIdx && j >= textStartEntryIdx && textIdx < textEntryCnt)	// 正文段映射
			{
				pUserPageTable[i].m_Entrys[j].m_Present = 1;
				pUserPageTable[i].m_Entrys[j].m_ReadWriter = 0;	// RO
				pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = textIdx + textPF;

				textIdx++;
			}
			
			if (i == rdataStartUserPageTabelIdx && j >= rdataStartEntryIdx && textIdx < textEntryCnt + rdataEntryCnt)	// 只读数据段映射
			{
				pUserPageTable[i].m_Entrys[j].m_Present = 1;
				pUserPageTable[i].m_Entrys[j].m_ReadWriter = 0;	// RO
				pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = textIdx + textPF;

				textIdx++;
			}
			
			if (i == dataStartUserPageTabelIdx && j >= dataStartEntryIdx && dataIdx < 1 + dataEntryCnt - rdataEntryCnt)	// 可交换部分第一页是 PPDA，所以要 + 1
			{
				if (j < rdataStartEntryIdx || j >= rdataStartEntryIdx + rdataEntryCnt)	// 排除只读数据段，只映射其他部分
				{
					pUserPageTable[i].m_Entrys[j].m_Present = 1;
					pUserPageTable[i].m_Entrys[j].m_ReadWriter = 1;	// RW
					pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = dataIdx + pAddrPF;

					dataIdx++;
				}
			}
			
			if (i == stackStartUserPageTabelIdx && j >= stackStartEntryIdx && dataIdx < 1 + dataEntryCnt - rdataEntryCnt + stackEntryCnt)	// 堆栈段映射
			{
				pUserPageTable[i].m_Entrys[j].m_Present = 1;
				pUserPageTable[i].m_Entrys[j].m_ReadWriter = 1;	// RW
				pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = dataIdx + pAddrPF;

				dataIdx++;
			}

		}
	}

	/* 最后，用户第一张页表的第一项 */

	pUserPageTable[0].m_Entrys[0].m_Present = 1;
	pUserPageTable[0].m_Entrys[0].m_ReadWriter = 1;
	pUserPageTable[0].m_Entrys[0].m_PageBaseAddress = 0;

	FlushPageDirectory();
}

