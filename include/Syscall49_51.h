#ifndef SYSCALL_49_51_H
#define SYSCALL_49_51_H

#include <sys/types.h>

/* =====================================================
 * 系统调用 49, 50, 51 的用户态接口定义
 * 用于用户程序调用内核的新增系统调用
 * ===================================================== */

/* =====================================================
 * 系统调用 49 - get_ppid
 * 功能：获取父进程的 PID 号
 * 参数：无
 * 返回值：父进程的 PID 号 (int)
 * ===================================================== */
#define get_ppid() \
	(int)({ \
		int __res; \
		__asm__ __volatile__ ( \
			"int $0x80" \
			: "=a" (__res) \
			: "0" (49) \
		); \
		__res; \
	})

/* =====================================================
 * 系统调用 50 - get_pids
 * 功能：获取当前进程和父进程的 PID 号
 * 参数：无
 * 返回值：整数，低16位为当前进程PID，高16位为父进程PID
 * 使用例：
 *   int result = get_pids();
 *   int my_pid = result & 0xFFFF;
 *   int parent_pid = (result >> 16) & 0xFFFF;
 * ===================================================== */
#define get_pids() \
	(int)({ \
		int __res; \
		__asm__ __volatile__ ( \
			"int $0x80" \
			: "=a" (__res) \
			: "0" (50) \
		); \
		__res; \
	})

/* =====================================================
 * 系统调用 51 - get_proc
 * 功能：获取进程的内存使用情况
 * 参数：
 *   text_addr_ptr - 用于返回代码段物理地址的指针
 *   data_addr_ptr - 用于返回数据/栈段物理地址（交换区地址）的指针
 *   text_size_ptr - 用于返回代码段大小的指针
 *   data_size_ptr - 用于返回数据段大小的指针
 *   stack_size_ptr - 用于返回栈段大小的指针
 * 返回值：0 表示成功，负值表示失败
 * 使用例：
 *   unsigned long text_addr, data_addr;
 *   unsigned int text_size, data_size, stack_size;
 *   int ret = get_proc(&text_addr, &data_addr, &text_size, &data_size, &stack_size);
 *   if (ret == 0) {
 *       printf("Text: 0x%lx, size: %u\n", text_addr, text_size);
 *   }
 * ===================================================== */
#define get_proc(text_addr, data_addr, text_size, data_size, stack_size) \
	(int)({ \
		int __res; \
		__asm__ __volatile__ ( \
			"int $0x80" \
			: "=a" (__res) \
			: "0" (51), "b" (text_addr), "c" (data_addr), \
			  "d" (text_size), "S" (data_size), "D" (stack_size) \
		); \
		__res; \
	})

#endif	/* SYSCALL_49_51_H */
