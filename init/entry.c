/*
 * =====================================================================================
 *
 *       Filename:  entry.c
 *
 *    Description:  入口函数
 *
 *        Version:  1.0
 *        Created:  2019.3.21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "console.h"
#include "debug.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
// #include "memory.h"
// #include "string.h"
// 
// #include "vmm.h"
// #include "heap.h"
// #include "sched.h"


void interrupt5_handler(pt_regs_t *temp)
{
		printk("This is %dth interrupt handler!\n",temp->int_no);
}
__attribute__((section(".init.text")))int kern_entry()
{
	// 初始化函数
	console_clear();
	init_debug();
	

	init_gdt();
	init_idt();
   	
/* -----------------------------------------------------测试中断处理----------------------------------------*/
	printk_color(rc_black,rc_yellow,"--------------------Test interrupt handler------------------\n");	
	printk_color(rc_black,rc_green,"\nHEllo,DTL kernel!\n");
	
	//printk("\n%X\n",mmap_information);
	
	interrupt_handler_t interrupt5 = interrupt5_handler;
	register_interrupt_handler(5,interrupt5);
	//printk("\n0x%X\n",idt_entries);

	asm volatile ("int $0x3");
	asm volatile ("int $0x4");
	asm volatile ("int $5");

	// init_timer(100);// 传入的就是实际的频率
	// enable_intr();

/* -------------------------------------------测试输出内存信息(从BIOS获取的)-----------------------------------*/
	printk_color(rc_black,rc_yellow,"------------------Output Memory information----------------\n");	
	//printk("\n%X\n",total_mem_bytes);
	printk_color(rc_black,rc_light_green,"\nTotal Memory Size: %d KB, %d MB\n",total_mem_bytes/1024,total_mem_bytes/1024/1024);
	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end: 0x%08X\n", kern_end);
	printk("kernel in memory used: %d KB\n\n", (kern_end-kern_start+1023)/1024);
	show_memory_map();
	
/* ---------------------------------------测试页面分配-----------------------------------------------*/
	printk_color(rc_black,rc_yellow,"-----------------Test page allocation-------------------\n");	
	init_pmm();
	
	printk_color(rc_black,rc_light_magenta,"\nThe number of Physical Memory Pages is %u \n\n",phy_page_count);
	// 测试分配页面
	int32_t new_alloc_page=NULL;
	new_alloc_page = pmm_alloc_page();
	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
	new_alloc_page = pmm_alloc_page();
	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
	new_alloc_page = pmm_alloc_page();
	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
	new_alloc_page = pmm_alloc_page();
	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);

/* ---------------------------------------------测试虚拟地址映射和访问----------------------------------------*/
	printk_color(rc_black,rc_yellow,"-------------------Test virtual memory mapping-------------------\n");	
	init_vmm();
	int32_t test_virtual_page = 0x90000000;
	int32_t test_phy_page = 0x80000;
	int32_t *test_result = 0x70000;
	printk_color(rc_black,rc_light_red,"Now 0x%X 's physical address is: 0x%X",test_virtual_page,get_mapping(pgd_kern,test_virtual_page,test_result));
	printk("\n----------After mapping-------\n");
	map(pgd_kern,test_virtual_page,test_phy_page,PAGE_PRESENT|PAGE_WRITE|PAGE_USER);
	printk_color(rc_black,rc_light_red,"Now 0x%X 's physical address is: 0x%X\n",test_virtual_page,get_mapping(pgd_kern,test_virtual_page,test_result));
	printk("So the test_result address is :0x%X\n",*test_result);
	printk("I write 5 'D' at virtual address 0x%X and then I read the physical address 0x%X and print them\n",test_virtual_page,test_phy_page);
	memset(test_virtual_page,'D',5);
	memset(test_phy_page+5,0,2);
	char *test_str= (char*)test_virtual_page;
	printk_color(rc_black,rc_light_blue,"now 0x%X 's content is :%s\n",test_virtual_page,*test_result);
	printk("\n---------------------After unmapping------------\n");
	unmap(pgd_kern,test_virtual_page);
	printk_color(rc_black,rc_light_red,"Now 0x%X 's physical address is: 0x%X\n",test_virtual_page,get_mapping(pgd_kern,test_virtual_page,test_result));
	printk("Now the map is ending, So if I continue to read this virtual address,a page error will occur\n");
	char *error_test= (char*)test_virtual_page;
	printk_color(rc_black,rc_light_blue,"now 0x%X 's content is :%s\n",test_virtual_page,*error_test);
	


/* ----------------------------------TEST SOME DEBUG FUNCTION TRACE THE FUNCTION STACK------------*/
	panic("test");
	
	return 0;
}

