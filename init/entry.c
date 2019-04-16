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
#include "gdt.h"
#include "console.h"
#include "debug.h"
#include "idt.h"
#include "timer.h"
#include "string.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "sched.h"

// 内核初始化函数
void kern_init();

// 开启分页机制之后的Multiboot 数据指针
multiboot_t *glb_mboot_ptr;

// 开启分页之后的内核栈
uint8_t kern_stack[STACK_SIZE]  __attribute__ ((aligned(STACK_SIZE)));

// 内核栈的栈顶
uint32_t kern_stack_top = (uint32_t)kern_stack + STACK_SIZE;

// 内核使用的临时页表和页目录
// 改地址必须是页对齐的地址，内存0--640KB肯定是空闲的
// 连着定义三个地址，这是三个连续的页从0x1000--0x3000
// pgd_tmp是临时页目录
__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
// 下边两个分别是两个页表，一个存放低地址，一个存放高地址(地址映射之后的，就是0xc0000000之后的)
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;

// 内核函数入口
__attribute__((section(".init.text"))) void kern_entry()
{

	
	// 页目录的第一项，存放第一个页表的地址
	pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE ;
	// 页目录的第PGD_INDEX(PAGE_OFFSET)项，存放0xc0000000对应的页表地址
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_high | PAGE_PRESENT | PAGE_WRITE;

	// 映射内核虚拟地址 4MB 到物理地址的前4MB
	int i;
	for (i=0; i<1024; i++) {
		pte_low[i] = (i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	// 映射 虚拟地址 0xc0000000 --- 0xc0400000 到 0x00000000 --- 0x00400000的物理地址
	for (i=0;i<1024;i++) {
		pte_high[i] = (i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	// 设置临时页表
	asm volatile ("mov %0,%%cr3" : : "r" (pgd_tmp));

	uint32_t cr0;

	//启用分页， 将cr0寄存器的分页位设置为1
	asm volatile ("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r"(cr0));

	// 切换内核栈
	uint32_t kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xfffffff0;
	asm volatile ("mov %0, %%esp; xor %%ebp, %%ebp" : : "r"(kern_stack_top));

	// 更新全局multiboot_t指针
	glb_mboot_ptr = mboot_ptr_tmp+PAGE_OFFSET;

	// 调用内核初始化函数
	kern_init();
}

int flag=0;

int thread(void *arg)
{
	while(1) {
		if (flag==1) {
			printk_color(rc_black,rc_green,"B");
			flag=0;
		}
	}
	return 0;
}

void kern_init()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n\n");

	init_timer(200);

	//开启中断
	//asm volatile("sti");

	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end: 0x%08X\n", kern_end);
	printk("kernel in memory used: %d KB\n\n", (kern_end-kern_start+1023)/1024);

	show_memory_map();
	init_pmm();
	init_vmm();

	printk_color(rc_black,rc_light_magenta,"\nThe number of Physical Memory Pages is %u \n\n",phy_page_count);

	uint32_t allc_addr=NULL;
	printk_color(rc_black,rc_yellow, "Test Physical Memory Alloc :\n");
	allc_addr = pmm_alloc_page();
	printk_color(rc_black,rc_yellow, "Alloc Physical Addr: 0x%08X\n",allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black,rc_yellow, "Alloc Physical Addr: 0x%08X\n",allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black,rc_yellow, "Alloc Physical Addr: 0x%08X\n",allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black,rc_yellow, "Alloc Physical Addr: 0x%08X\n",allc_addr);

	test_heap();

	init_sched();

	kernel_thread(thread,NULL);

	//开启中断
	enable_intr();
	//asm volatile("sti");

	while(1) {
		if(flag==0) {
			printk_color(rc_black,rc_red,"A");
			flag=1;
		}
	}

	while(1){
		asm volatile("hlt");
	}
}

// int kern_entry()
// {
// 	console_clear();

// 	/*uint8_t str[]="Hello DTL!This is Your OS!\nIn this section of the tutorial, we will delve into a bit of assembler,\ 
// 	learn the basics of creating a linker script as well as the reasons for using one, \
// 	and finally, we will learn how to use a batch file to automate the assembling, \
// 	compiling, and linking of this most basic protected mode kernel. \
// 	Please note that at this point, the tutorial assumes that you have NASM and DJGPP installed on a Windows or DOS-based platform.\ 
// 	We also assume that you have a a minimal understanding of the x86 Assembly language.\
// 	The Kernel EntryIn this section of the tutorial, we will delve into a bit of assembler,\ 
// 	learn the basics of creating a linker script as well as the reasons for using one, \
// 	and finally, we will learn how to use a batch file to automate the assembling, \
// 	compiling, and linking of this most basic protected mode kernel. \
// 	Please note that at this point, the tutorial assumes that you have NASM and DJGPP installed on a Windows or DOS-based platform.\ 
// 	We also assume that you have a a minimal understanding of the x86 Assembly language.\In this section of the tutorial, we will delve into a bit of assembler,\ 
// 	learn the basics of creating a linker script as well as the reasons for using one, \
// 	and finally, we will learn how to use a batch file to automate the assembling, \
// 	compiling, and linking of this most basic protected mode kernel. \
// 	Please note that at this point, the tutorial assumes that you have NASM and DJGPP installed on a Windows or DOS-based platform.\ 
// 	We also assume that you have a a minimal understanding of the x86 Assembly language.\In this section of the tutorial, we will delve into a bit of assembler,\ 
// 	learn the basics of creating a linker script as well as the reasons for using one, \
// 	and finally, we will learn how to use a batch file to automate the assembling, \
// 	compiling, and linking of this most basic protected mode kernel. \
// 	Please note that at this point, the tutorial assumes that you have NASM and DJGPP installed on a Windows or DOS-based platform.\ 
// 	We also assume that you have a a minimal understanding of the x86 Assembly language.";*/

// 	//console_write_color(str,rc_dark_grey,rc_yellow);
// 	// int num=100;
// 	// char str[]="hello dtl";
// 	// // for(i=0;i<100;i++)
// 	// // {
// 	// // 	console_write_hex(i,rc_black,rc_light_green);
// 	// // 	console_write("    ");
// 	// // 	console_write_dec(i,rc_black,rc_yellow);
// 	// // 	console_write("    ");
// 	// // }
// 	// printk("%d\n%s\n",num,str);
// 	// init_debug();
// 	// console_clear();
// 	// printk_color(rc_black,rc_green,"Hello, DTL kernel!\n");
// 	// panic("test");
// 	init_debug();
// 	init_gdt();
// 	init_idt();

// 	console_clear();
// 	printk_color(rc_black,rc_green,"HEllo,DTL kernel!\n");

// 	void interrupt5_handler(pt_regs *temp)
// 	{
// 		printk("This is %dth interrupt handler!\n",temp->int_no);
// 	}
	
// 	interrupt_handler_t interrupt5=interrupt5_handler;
// 	register_interrupt_handler(5,interrupt5);

// 	asm volatile ("int $0x3");
// 	asm volatile ("int $0x4");
// 	asm volatile ("int $5");

// 	init_timer(100);// 传入的就是实际的频率
// 	//asm volatile ("sti");

// 	printk("kernel in memory start: 0x%08X\n", kern_start);
// 	printk("kernel in memory end: 0x%08X\n", kern_end);
// 	printk("kernel in memory used: %d KB\n\n", (kern_end-kern_start+1023)/1024);
// 	show_memory_map();

// 	init_pmm();
// 	printk_color(rc_black,rc_light_magenta,"\nThe number of Physical Memory Pages is %u \n\n",phy_page_count);
// 	// 测试分配页面
// 	int32_t new_alloc_page=NULL;
// 	new_alloc_page = pmm_alloc_page();
// 	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
// 	new_alloc_page = pmm_alloc_page();
// 	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
// 	new_alloc_page = pmm_alloc_page();
// 	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);
// 	new_alloc_page = pmm_alloc_page();
// 	printk_color(rc_black,rc_light_cyan,"Allocate Physical Address: 0x%08X\n",new_alloc_page);

// 	return 0;
// }

