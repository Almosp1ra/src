#include <sys.h>
#include <malloc.h>
#include <stdio.h>

#define PAGE_SIZE 12288

char *malloc_begin = NULL;
char *malloc_end = NULL;

typedef struct flist {
   unsigned int size;
   struct flist *nlink;
   struct flist *plink;
};

struct flist *malloc_head = NULL;

void* malloc(unsigned int size)
{
    if (malloc_begin == NULL)
    {   // 1、首次执行malloc，启用堆
        malloc_begin = sbrk(0);
        malloc_end = sbrk(PAGE_SIZE);
        malloc_head = malloc_begin;
        malloc_head->size = sizeof(struct flist);
        malloc_head->nlink = NULL;
        malloc_head->plink = NULL;  // 设置首片的prev指针
    }
    if (size == 0)
    {
        return NULL;
    }
    size += sizeof(struct flist);
    size = ((size + 7) >> 3) << 3;
    struct flist* iter = malloc_head;

    // 2、分配动态内存（找一个足够大的空闲片）
    struct flist *temp;
    while( iter->nlink != NULL )
    {
        if ((int)(iter->nlink) - iter->size - (int)iter >= size)
        {
            temp = (char *)iter + (iter->size);
            temp->nlink = iter->nlink;
            iter->nlink = temp;
            temp->plink = iter;  // 设置prev指针
            temp->nlink->plink = temp;
            temp->size = size;
            return (char *)temp + sizeof(struct flist);
        }
        iter = iter->nlink;
    }

    int remain;
L1: remain = malloc_end - iter->size - (int)iter;
    if ( remain >= size)
    {   // 最后一个内存片之后的空间，足够吗？
        temp = (char *)iter + (iter->size);
        temp->nlink = NULL;
        iter->nlink = temp;
        temp->plink = iter;  // 设置prev指针
        temp->size = size;
        return (char *)temp + sizeof(struct flist);
    }

    // 3、内存分配不成功，执行sbrk系统调用，扩展堆空间
    int expand = size - remain;
    expand = ((expand + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;  // 扩展3页的整数倍
    malloc_end = sbrk(expand);
    goto L1;  // 分配
}

int free(void* addr)  // 释放以addr为首地址的动态内存
{
    if (addr == 0)
    {
        return -1;
    }
    struct flist* block = (struct flist*)((char*)addr - sizeof(struct flist));   // 直接定位释放内存片

    block->plink->nlink = block->nlink;
    if (block->nlink != NULL)
    {
        block->nlink->plink = block->plink;
    }
    else
    {
        char *pos = (char *)block + block->size;
        if (malloc_end - pos > PAGE_SIZE * 2)
        {
            malloc_end = sbrk(-((malloc_end - pos) / PAGE_SIZE * PAGE_SIZE));
        }
    }
    return 0;
}


