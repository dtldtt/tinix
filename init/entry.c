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
#include "init.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "keyboard.h"
#include "test.h"
 
// #include "memory.h"
// #include "string.h"
// 
// #include "vmm.h"
// #include "heap.h"
// #include "sched.h"
extern char* input_buffer;
extern int input_pos;



__attribute__((section(".init.text")))int kern_entry()
{
	init_all();
	enable_intr();
	welcome();
/* -----------------------------------------------------测试中断处理----------------------------------------*/
// 	test_interrupt();

// /* -------------------------------------------测试输出内存信息(从BIOS获取的)-----------------------------------*/
// 	test_memory_info();
	
// /* ---------------------------------------测试页面分配-----------------------------------------------*/
// 	test_page_alloc();
// /* ---------------------------------------------测试虚拟地址映射和访问----------------------------------------*/
// 	test_virtual_memory();
// /* ---------------------------------------------测试内核堆分配----------------------------------------*/
// 	test_kmalloc();
	
/* -----------------------------------Test multiple Threads switch ------------------------------*/
	// printk_color(rc_black,rc_yellow,"--------------------Test multiple Threads switch -------------------\n");
	// kernel_thread(thread,NULL);
	// enable_intr();
	// while(1){
	// 	if(flag==0){
	// 		printk_color(rc_white,rc_red,"A");
	// 		flag=1;
	// 	}
	// }

/* ------------------------------------Test Keyboard Interrupt Handler --------------------------------*/
	// kernel_thread(thread0,NULL);
	// kernel_thread(thread1,NULL);
	// enable_intr();
	// char str[64];
	// getline(str);
	// if(!strcmp(str,"input")){
	// 	test_input();
	// }
	
	
	/* ----------------------------------TEST SOME DEBUG FUNCTION TRACE THE FUNCTION STACK------------*/
	//panic("test");

	return 0;
	
}

