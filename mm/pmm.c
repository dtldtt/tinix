/*
 * =====================================================================================
 *
 *       Filename:  pmm.c
 *
 *    Description:  页内存管理的实现page memory management
 *
 *        Version:  1.0
 *        Created:  2019.3.25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#include "common.h"
#include "debug.h"
#include "pmm.h"

// 物理内存页面管理的栈
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];

// 物理内存管理的栈指针
static uint32_t pmm_stack_top=0;

// 物理内存页的数量
uint32_t phy_page_count=0;

void show_memory_map()
{
    uint32_t mmap_addr=mmap_information->mmap_addr;
    uint32_t mmap_length=mmap_information->mmap_length;
    //printk("mmap addr:%X,   mmap_length:%X\n",mmap_addr,mmap_length);

    printk_color(rc_black,rc_light_magenta,"Memory map: \n");

    mmap_entry_t *mmap=(mmap_entry_t *)mmap_addr;
    //printk("mmap addr:%X,   mmap_length:%X\n",mmap_addr,mmap_length);
    mmap_entry_t *initial_mmap=mmap;
    for(mmap=(mmap_entry_t *)mmap_addr; mmap<initial_mmap+mmap_length;mmap++)
    // for(i=0;i<mmap_length;i++)
    {
        printk("Memory %2d  ",(uint32_t)(mmap-initial_mmap));
        printk("base_addr = 0x%08X, length = 0x%08X, type = %08X\n",
       (uint32_t)(mmap->base_addr_low),(uint32_t)(mmap->length_low),(uint32_t)(mmap->type));
       //mmap++;
    }
}

// 初始化页管理，把内核结束位置到内存边界的内存分成页放在栈里边
void init_pmm()
{
    mmap_entry_t *mmap_start_addr = (mmap_entry_t *)(mmap_information->mmap_addr);
    mmap_entry_t *mmap_end_addr= (mmap_entry_t*)(mmap_information->mmap_addr) + mmap_information->mmap_length;

    mmap_entry_t *map_entry;

    for(map_entry=mmap_start_addr; map_entry< mmap_end_addr; map_entry++)
    {
        // 如果是可用内存 (type=1，其他类型指向保留区域)
        if (map_entry->type==1 && map_entry->base_addr_low==0x100000)
        {
            printk("Enter the for loop\n");
            // 把内核结束位置(base_addr_low + (uint32_t)(kern_end-kern_start))到内存结束位置的内存段
            // 按照页存储到页管理栈里
            // 最多支持128MB内存，这个主要看PMM_MAX_SIZE设置了多大
            uint32_t page_addr = map_entry->base_addr_low + (uint32_t)(kern_end-kern_start);
            uint32_t length = map_entry->base_addr_low + map_entry->length_low;

            while (page_addr < length && page_addr <= PMM_MAX_SIZE)
            {
                pmm_free_page(page_addr);
                page_addr += PMM_PAGE_SIZE;
                phy_page_count++;
            }
        }
    }
}

uint32_t pmm_alloc_page()
{
    assert(pmm_stack_top!=0,"Out of Memory!\n");
    uint32_t temp_page = pmm_stack[pmm_stack_top--];
    return temp_page;
}

void pmm_free_page(uint32_t p)
{
    assert(pmm_stack_top!=PAGE_MAX_SIZE,"Out of Page Stack!\n");
    pmm_stack[++pmm_stack_top]=p;
}