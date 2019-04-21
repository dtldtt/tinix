/*
 * =====================================================================================
 *
 *       Filename:  idt.c
 *
 *    Description:  idt设置相关实现
 *
 *        Version:  1.0
 *        Created:  2019.3.24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#include "common.h"
#include "string.h"
#include "debug.h"
#include "idt.h"



// IDTR
idt_ptr_t idt_ptr;

// 中断处理函数的指针数组
interrupt_handler_t interrupt_handlers[256];

// 设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t offset,uint16_t sel,uint8_t flags);

// 声明加载IDTR的函数
extern void idt_flush(uint32_t);

// 初始化中断描述符表
void init_idt()
{
    /** 设置8259A芯片
     * 重新映射IRQ表
     * 两片级联的8259A芯片
     * 主片端口0x20 0x21
     * 从片端口0xa0 0xa1
     * 
     * 其实就是初始化ICW1-ICW4
     * 具体参考链接：
     * https://blog.csdn.net/longintchar/article/details/79439466
     * 参照LINUX0.11版本的设置
     **/

    //初始化主片从片，0001 0001
    outb(0x20,0x11);
    outb(0xa0,0x11);

    // 设置主片IRQ从0x20(32)号中断开始
    outb(0x21,0x20);

    //设置从片IRQ从0x28(40)号中断开始
    outb(0xa1,0x28);

    // 设置主片IR2引脚连接从片
    outb(0x21,0x04);

    // 设置从片输出引脚和主片IR2号连接
    outb(0xa1,0x04);

    // 设置主片和从片按照8086的方式工作
    outb(0x21,0x01);
    outb(0xa1,0x01);

    // 设置主片允许中断
    outb(0x21,0);
    outb(0xa1,0);


    //所有中断处理函数在中断处理函数数组中初始化为0
    bzero((uint8_t *)&interrupt_handlers,sizeof(interrupt_handler_t) * 256);

    idt_ptr.limit= sizeof(idt_entry_t) * 256 -1 ;
    idt_ptr.base=(uint32_t) idt_entries;

    bzero((uint8_t *)idt_entries,sizeof(idt_entry_t)*256);

    // 0-31: 用于CPU的中断处理，具体数值查看中断门结构
    idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
 
// 32-47 IRQ中断
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
    // 255 用于系统调用
    idt_set_gate(255, (uint32_t)isr255, 0x08, 0x8E);

    // 更新中段描述符表
    idt_flush((uint32_t)&idt_ptr);
}

// 设置中段描述符
static void idt_set_gate(uint8_t num, uint32_t offset, uint16_t sel, uint8_t flags)
{
    idt_entries[num].offset_low=offset & 0xffff;
    idt_entries[num].offset_high=(offset>>16) & 0xffff;

    idt_entries[num].selector=sel;
    idt_entries[num].always0=0;

    //0x60留作以后实现用户态时候使用
    //这个或运算可以设置中断们的特权级别为3
    idt_entries[num].flags=flags; // | 0x60
}

//调用中断处理函数
void isr_handler(pt_regs_t *regs)
{
	if (interrupt_handlers[regs->int_no]) {
	      interrupt_handlers[regs->int_no](regs);
	} else {
		printk_color(rc_black, rc_light_blue, "Unhandled interrupt: %d\n", regs->int_no);
	}
}

// IRQ处理函数
void irq_handler(pt_regs_t *regs)
{
    // 发送中断结束信号给 PICS
    // 按照设置，从32号中断开始是用户自定义中断
    // 单片8259A芯片只能处理8级中断
    // 所以大于等于40的由从片处理

    if(regs->int_no >= 40) {
        // 发送结束信息给从片，表示中断处理结束可以响应其他中断，其实就是重置，可以响应当前中断
        outb(0xa0,0x20);
    }
    // 发送结束信息给主片，同上
    outb(0x20,0x20);
    // 调用中断处理函数
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }
}

//注册一个中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h)
{
	interrupt_handlers[n] = h;
}

void enable_intr()
{
    asm volatile ("sti");
}

void unable_intr()
{
    asm volatile ("cli");
}


