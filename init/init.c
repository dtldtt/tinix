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

void init_all()
{
    // 初始化函数
	asm volatile ("mov %%esp,%0": "=r"(kern_stack));
	kern_stack_top = kern_stack;
	console_clear();
	init_debug();
	init_gdt();
	init_idt();
    init_pmm();
    init_vmm();
    init_heap();
   	init_sched();
	init_timer(100);
}