/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  内核功能测试相关函数定义
 *
 *        Version:  1.0
 *        Created:  2019.5.04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "test.h"
#include "console.h"
#include "debug.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "keyboard.h"
#include "init.h"

// 输入缓冲区大小
#define BUFFER_SIZE 64

static void interrupt5_handler(pt_regs_t *temp)
{
		printk("This is %dth interrupt handler!\n",temp->int_no);
}

static back_to_homepage()
{
	printk_color(rc_black,rc_brown,"Please input ");
	printk_color(rc_black,rc_red,"quit");
	printk_color(rc_black,rc_brown," and press Enter to return Homepage!\n");
	char str[BUFFER_SIZE];
	while(1){
		getline(str);
		if(!strcmp(str,"quit")){
			break;
		} else {
			printk_color(rc_black,rc_red,"Invalid Input! Please input quit and press Enter!\n");
		}
	}
	welcome();
}

static bool flag=0;


static int thread(void *arg)
{
	int count=0;
	while(1){
		if(count>499) break;
		if(flag==1){
			printk_color(rc_black,rc_green,"B");
			count++;
			flag=0;
		}
	}
	while(1);
	return 0;
}

void test_interrupt()
{
	console_clear();
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
	back_to_homepage();
}

void test_memory_info()
{
	console_clear();
    printk_color(rc_black,rc_yellow,"------------------Output Memory information----------------\n");	
	//printk("\n%X\n",total_mem_bytes);
	printk_color(rc_black,rc_light_green,"\nTotal Memory Size: %d KB, %d MB\n",total_mem_bytes/1024,total_mem_bytes/1024/1024);
	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end: 0x%08X\n", kern_end);
	printk("kernel in memory used: %d KB\n\n", (kern_end-kern_start+1023)/1024);
	show_memory_map();
	//while(1);
	back_to_homepage();
}

void test_page_alloc()
{
	console_clear();
    printk_color(rc_black,rc_yellow,"-----------------Test page allocation-------------------\n");	
	
	
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

	back_to_homepage();
}

void test_virtual_memory()
{
	console_clear();
    printk_color(rc_black,rc_yellow,"-------------------Test virtual memory mapping-------------------\n");	
	
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
	printk_color(rc_black,rc_light_green,"now 0x%X 's content is :%s\n",test_virtual_page,*test_result);
	// printk("\n---------------------After unmapping------------\n");
	// unmap(pgd_kern,test_virtual_page);
	// printk_color(rc_black,rc_light_red,"Now 0x%X 's physical address is: 0x%X\n",test_virtual_page,get_mapping(pgd_kern,test_virtual_page,test_result));
	// printk("Now the map is ending, So if I continue to read this virtual address,a page error will occur\n");
	// char *error_test= (char*)test_virtual_page;
	// printk_color(rc_black,rc_light_blue,"now 0x%X 's content is :%s\n",test_virtual_page,*error_test);

	back_to_homepage();
}

void test_kmalloc()
{
	console_clear();
    printk_color(rc_black,rc_yellow,"--------------------Test kmalloc() and kfree() -------------------\n");

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

	back_to_homepage();
}

void test_multi_threads()
{
	console_clear();
	printk_color(rc_black,rc_yellow,"--------------------Test multiple Threads switch -------------------\n");
	kernel_thread(thread,NULL);
	int count=0;
	while(1){
		if(count>500) break;
		if(flag==0){
			printk_color(rc_black,rc_red,"A");
			flag=1;
			count++;
		}
	}

	back_to_homepage();

}