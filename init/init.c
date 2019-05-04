/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  内核初始化函数实现
 *
 *        Version:  1.0
 *        Created:  2019.4.23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "init.h"
// 输入缓冲区的大小
#define BUFFER_SIZE 64
// 全局输入缓冲区
char input_buffer[BUFFER_SIZE];
// 标识缓冲区中字符个数
int input_pos;

void init_all()
{
    // 初始化函数
	asm volatile ("mov %%esp,%0": "=r"(kern_stack));
	kern_stack_top = kern_stack;
	bzero(input_buffer,BUFFER_SIZE);
	input_pos=0;
	console_clear();
	init_debug();
	init_gdt();
	init_idt();
    init_pmm();
    init_vmm();
    init_heap();
   	init_sched();
	init_timer(100);
	init_keyboard();
}

static void choose_one()
{
	char str[BUFFER_SIZE];
	while(1){
		getline(str);
		if(!strcmp(str,"1")){
		test_interrupt();
		}
		else if(!strcmp(str,"2")){
			test_memory_info();
		}
		else if(!strcmp(str,"3"))
		{
			test_page_alloc();
		}
		else if(!strcmp(str,"4")){
			test_virtual_memory();
		}
		else if(!strcmp(str,"5")){
			test_kmalloc();
		}else if(!strcmp(str,"6")){
			test_multi_threads();
		}else if(!strcmp(str,"7")){
			test_panic();
		}
		else{
			printk_color(rc_white,rc_red,"Invalid Input!Please choose one number and press Enter\n");
		}
	}
	
}

void welcome()
{
	console_clear();
	printk_color(rc_black,rc_green,"--------------------Welcome to DTL-OS:Tinix------------------\n");
	printk("\nPlease choose a number below to test a function in Tinix:\n");
	printk("Thers are many functions in Tinix,such as:\n");
	printk_color(rc_black,rc_yellow,"<1> Test Interrupt handler in Tinix\n");
	printk_color(rc_black,rc_yellow,"<2> Output Memory Information about this computer\n");
	printk_color(rc_black,rc_yellow,"<3> Test Page Allocation in Tinix\n");
	printk_color(rc_black,rc_yellow,"<4> Test Virtual Memory Mapping in Tinix\n");
	printk_color(rc_black,rc_yellow,"<5> Test Dynamic size memory block Allocation in Tinix\n");
	printk_color(rc_black,rc_yellow,"<6> Test Multiple Threads Switch in Tinix\n");
	printk_color(rc_black,rc_yellow,"<7> Output Function Calling Stack, It is used in debug and kernel development\n");
	printk("\nPlease one of the options above\n");
	printk("For example, you can input 1 and then press Enter to test interrupt handler\n");

	choose_one();
}