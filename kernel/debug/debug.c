/*
 * =====================================================================================
 *
 *       Filename:  debug.c
 *
 *    Description:  调试相关的实现
 *
 *        Version:  1.0
 *        Created:  2019.3.23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#include "debug.h"

static void print_stack_trace();
static elf_t kernel_elf;

void init_debug()
{
    //从GRUB提供的信息中获取到内核符号表和代码地址的信息
    //printk("\nsection_header_table:%X\n",section_header_table);
    kernel_elf= elf_from_multiboot(section_header_table);
}

void print_cur_status()
{
    static int round=0;
    uint16_t reg1,reg2,reg3,reg4;

    asm volatile (  "mov %%cs,%0;"
                    "mov %%ds,%1;"
                    "mov %%es,%2;"
                    "mov %%ss,%3;"
                    :   "=m"(reg1),"=m"(reg2),"=m"(reg3), "=m"(reg4)
                );
    //打印当前的运行级别
    printk("%d: @ring %d\n",round,reg1 & 0x3);
    printk("%d: cs=%x\n",round,reg1);
    printk("%d: ds=%x\n",round,reg2);
    printk("%d: es=%x\n",round,reg3);
    printk("%d: ss=%x\n",round,reg4);
    ++round;
}
// flag为1时为了展示，不无限循环
void panic(const char *msg,bool flag)
{
    printk_color(rc_black,rc_red,"********************* SYSTEM panic: %s *****************\n", msg);
    print_stack_trace();
    printk_color(rc_black,rc_red,"************************\n");
    if(flag) return;
    //致命错误发生猴打印栈信息然后停止在这里
    while(1);
}
// 这里用EBP是因为调用函数后x86会自动push ebp，相当于ebp保存着返回后的下一条指令的地址，具体可查找x86函数调用的相关过程
void print_stack_trace()
{
    uint32_t *ebp,*eip;

    asm volatile ("mov %%ebp,%0": "=r"(ebp));
    while(ebp) {
        eip=ebp+1;
        printk("    [0x%x] %s\n",*eip,elf_lookup_symbol(*eip,&kernel_elf));
        ebp=(uint32_t*)*ebp;
    
    }
    
}