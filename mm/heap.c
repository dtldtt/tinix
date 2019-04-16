/*
 * =====================================================================================
 *
 *       Filename:  heap.c
 *
 *    Description:  内存堆管理的实现
 *
 *        Version:  1.0
 *        Created:  2019.4.10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "debug.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

/**
 * 以下函数为内核堆管理内部使用函数，所以设置为static
 */ 

// 申请内存块
static void alloc_block(uint32_t start, uint32_t len);

// 释放内存块
static void free_block(header_t *block);

// 切分内存块
static void split_block(header_t *block, uint32_t len);

// 合并内存块
static void merge_block(header_t *block);

static uint32_t heap_max = HEAP_START;

// 内存块管理头指针
static header_t *heap_first;

void init_heap()
{
    heap_first=0;
}

void* kmalloc(uint32_t len)
{
    // 因为这个len是要申请的长度，不算头部
    // 所以在实际的申请中要加上头部的长度sizeof(header_t)
    len += sizeof(header_t);

    header_t *cur_header = heap_first;
    header_t *prev_header = 0;

    // 循环查找长度够的而且未被使用的块
    while(cur_header) {
        // 如果有这样的内存块
        if (cur_header->allocated == 0 && cur_header->length >= len){
            // 切割内存块
            split_block(cur_header,len);
            cur_header->allocated=1;
            // 返回实际内存块的首地址，而不是内存块header的地址
            return (void*)((uint32_t)cur_header + sizeof(header_t));
        }
        // 没有的话继续循环查找
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    // 如果第一次调用该函数，则需要申请内存块
    // 因为此时没有任何已经申请但未被使用的内存块
    uint32_t block_start;
    // 如果prev_header有效，则表示不是第一次访问该函数
    // 而是因为现存的内存块没有合适的
    if (prev_header) {
        block_start=(uint32_t)prev_header + prev_header->length;
    } else {
        block_start = HEAP_START;
        heap_first = (header_t*)block_start;
    }

    // 分配新的内存块
    alloc_block(block_start,len);
    cur_header = (header_t *)block_start;
    cur_header->prev = prev_header;
    cur_header->next = 0;
    cur_header->allocated=1;
    cur_header->length=len; //内存块长度加上头部长度

    if(prev_header) {
        prev_header->next = cur_header;
    }

    return (void*)(block_start + sizeof(header_t));
}

void kfree(void *p)
{
    // allocated置零，并进行相邻块的合并操作
    // p-头部长度找到该内存块header地址
    header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
    header->allocated=0;

    // 合并内存块
    merge_block(header);
}

void alloc_block(uint32_t start, uint32_t len)
{
    // 如果当前堆的位置已经到了该页的边界就申请新的内存页
    // 循环申请知道满足请求的大小
    while(start+len>heap_max){
        uint32_t page = pmm_alloc_page();
        map(pgd_kern, heap_max,page,PAGE_PRESENT | PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

void free_block(header_t *block)
{
    // 如果释放的是第一块内存块
    if(block->prev==0) {
        heap_first=0;
    } else {    //否则上一块的下一块置零（这块释放了）
        block->prev->next=0;
    }

    // 空闲的内存超过1页的话就释放掉
    while ((heap_max-PAGE_SIZE) >= (uint32_t)block) {
        heap_max-=PAGE_SIZE;
        uint32_t page;
        get_mapping(pgd_kern, heap_max,&page);
        unmap(pgd_kern,heap_max);
        pmm_free_page(page);
    }
}

void split_block(header_t* block,uint32_t len)
{
    // 切分内存块之前得保证剩下的内存至少容纳一个内存管理快的大小（大于header的大小）
    if(block->length-len > sizeof(header_t)) {
        header_t *new_block=(header_t *)((uint32_t)block + block->length);
        new_block->prev=block;
        new_block->next=block->next;
        new_block->allocated=0;
        new_block->length=block->length-len;

        block->next=new_block;
        block->length=len;
    }
}

void merge_block(header_t* block)
{
    // 把该内存块与前边或者后边的空闲内存块合并成更大的内存块
    if (block->next && block->next->allocated==0)
    {
        block->length = block->length + block->next->length;
        if(block->next->next) {
            block->next->next->prev=block;
        }
        block->next=block->next->next;
    }

    if (block->prev && block->prev->allocated==0) {
        block->prev->length=block->prev->length+block->length;
        block->prev->next=block->next;
        if(block->next)
        {
            block->next->prev=block->prev;
        }
        block=block->prev;
    }

    // 如果这是最后一块内存块，就直接释放
    if (block->next==0)
    {
        free_block(block);
    }
}

void test_heap()
{
    printk_color(rc_black,rc_magenta,"Test kmalloc() and kfree() now ...\n");

    void *addr1 = kmalloc(50);
    printk("kmalloc 50 byte in 0x%X\n",addr1);
    void *addr2 = kmalloc(500);
    printk("kmalloc 500 byte in 0x%X\n",addr2);
    void *addr3 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%X\n",addr3);
    void *addr4 = kmalloc(50000);
    printk("kmalloc 50000 byte in 0x%x\n",addr4);
    
    printk("Free memory in 0x%X\n",addr1);
    kfree(addr1);
    printk("Free memory in 0x%X\n",addr2);
    kfree(addr2);
    printk("Free memory in 0x%X\n",addr3);
    kfree(addr3);
    printk("Free memory in 0x%X\n",addr4);
    kfree(addr4);
}
