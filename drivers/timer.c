// /*
//  * =====================================================================================
//  *
//  *       Filename:  timer.c
//  *
//  *    Description:  定义 PIT(周期中断定时器) 相关函数实现
//  *
//  *        Version:  1.0
//  *        Created:  2019.3.24
//  *       Revision:  none
//  *       Compiler:  gcc
//  *
//  *         Author:  DTL (TONGLIANG DONG)
//  *
//  * =====================================================================================
//  */

// #include "timer.h"
// #include "debug.h"
// #include "common.h"
// #include "idt.h"
// #include "sched.h"

// // 暂时设定只有前5秒显示tick，后面不再显示
// void timer_callback(pt_regs *regs)
// {
// 	// static uint32_t tick = 0;
// 	// if(tick<=500){
// 	// 	printk_color(rc_black, rc_red, "Tick: %d\n", tick++);
// 	// }
// 	// else {
// 	// 	asm volatile ("cli");
// 	// }
// 	schedule();
// }


// // frequency表示多少HZ，100hz表示每10ms发出一次中断
// void init_timer(uint32_t frequency)
// {
// 	// 注册时间相关的处理函数
// 	register_interrupt_handler(IRQ0, timer_callback);

// 	// Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
// 	// 其中0x40~0x42分别对应计数器通道0~2，0x43对应控制字寄存器写端口。
// 	// 输入频率为 1193180，frequency 即每秒中断次数
// 	uint32_t divisor = 1193180 / frequency;

// 	// 控制字格式如下：对应端口0x43
// 	// D7 D6 D5 D4 D3 D2 D1 D0
// 	// 0  0  1  1  0  1  1  0
// 	// D0=0表示二进制格式，D2D1=11表示使用方式3
// 	// D5D4=11 表示先读写低字节再读写高字节
// 	// D7D6=00 表示计数器0
// 	// 具体定义参见：https://blog.csdn.net/longintchar/article/details/78885556
// 	// 即就是 36 H
// 	// 设置 8253/8254 芯片工作在模式 3 下
// 	outb(0x43, 0x36);

// 	// 拆分低字节和高字节
// 	uint8_t low = (uint8_t)(divisor & 0xFF);
// 	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

// 	uint8_t cur_low=inb(0x40);
// 	uint8_t cur_high=inb(0x40);
// 	int result= ((cur_high << 8) & 0xff00)+cur_low;
// 	printk("Latch is %d\n",result );
	
// 	// 分别写入低字节和高字节，先低后高
// 	outb(0x40, low);
// 	outb(0x40, high);
// 	cur_low=inb(0x40);
// 	cur_high=inb(0x40);
// 	result= ((cur_high << 8) & 0xff00)+cur_low;
// 	printk("Latch is %d\n",result );
// }