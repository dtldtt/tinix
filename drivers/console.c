/*
 * =====================================================================================
 *
 *       Filename:  console.c
 *
 *    Description:  屏幕操作的一些函数实现
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
#include "common.h"
#include "console.h"
//#include "vmm.h"

//显卡显存映射在内存中的起点为0xB8000
static uint16_t *video_memory=(uint16_t *) 0xB8000;

//屏幕光标的坐标x,y
static uint8_t cursor_x=0;
static uint8_t cursor_y=0;
static uint8_t hex_set[]="0123456789ABCDEF";
static uint8_t dec_set[]="0123456789";

//移动光标
static void move_cursor()
{
	//屏幕宽80高25,80*25
	uint16_t cursorLocation=cursor_y*80+cursor_x;
	/* 光标在的位置在显卡内部的两个光标寄存器中
	** 每个寄存器是8位的，合起来形成一个16位数值
	** 这个数值表示光标的位置
	** 当光标在屏幕右下角也就是最后一个位置时
	** 此值为25*80-1=1999
	** 这两个寄存器都是可读可写的
	** 索引寄存器的端口号为0x3d4
	** 向它写入一个值来指定内部的某个寄存器
	** 光标寄存器的索引值分别是14和15,
	** 分别存储光标位置的高8位和低8位
	** 可以通过端口0x3d5对其内容进行读写
	*/
	outb(0x3d4,14);					//准备设置光标的高8位
	outb(0x3d5,cursorLocation>>8);	//设置光标高8位
	outb(0x3d4,15);					//准备设置光标的低8位
	outb(0x3d5,cursorLocation);		//设置光标低8位
}

//清屏
void console_clear()
{
	uint8_t attribute_byte=0x07; //黑底白字
	uint16_t blank=0x20 | (attribute_byte << 8); //一个字，表示黑底白字的空格，用来清屏
	
	int i;
	for (i=0;i<80*25;i++)
	{
		video_memory[i]=blank;
	}

	cursor_x=0;
	cursor_y=0;
	move_cursor();
}

//滚屏
static void scroll()
{
	/* 滚屏很简单，把前24行一次向前挪动一行
	** 最后一行用黑底白字的空格填充 */
	uint8_t attribute_byte=0x07;
	uint16_t blank=0x20 | (attribute_byte << 8);

	//cursor_y超过或者等于25时候需要滚屏
	if(cursor_y>=25)
	{
		int i;
		//除了第一行的每一行往前一行覆盖
		for (i=0;i<24*80;i++)
		{
			video_memory[i]=video_memory[i+80];
		}
		//最后一行填充
		for(i=24*80;i<25*80;i++)
		{
			video_memory[i]=blank;
		}
		//重置cursor_y
		cursor_y=24;
	}
}

//显示字符
void console_putc_color(char c, real_color_t back,real_color_t fore)
{
	uint8_t back_color=(uint8_t)back;
	uint8_t fore_color=(uint8_t)fore;

	uint8_t attribute_byte=(back_color << 4) | (fore_color & 0x0f);
	uint16_t attribute = attribute_byte<<8;

	//0x08是退格键的ASCII码
	//0x09是TAB键的ASCII码
	if (c==0x08 && cursor_x)
	{
		cursor_x--;
	}
	else if (c==0x09)
	{
		cursor_x=(cursor_x+4) & ~(4-1);//这里设置tab长度为4，使其4对其
	}
	else if (c=='\r')
	{
		cursor_x=0;
	}
	else if (c=='\n')
	{
		cursor_x=0;
		cursor_y++;
	}
	else if (c>=' ')
	{
		video_memory[cursor_y*80+cursor_x] = c | attribute;
		cursor_x++;
	}
	//如果显示到达行尾(x>=80)则自动换行
	if (cursor_x >= 80)
	{
		cursor_x=0;
		cursor_y++;
	}
	//滚屏，需不需要留给函数自己判断
	scroll();

	//移动光标
	move_cursor();
}

void console_write(char *cstr)
{
	while(*cstr) 
	{
		console_putc_color(*cstr++,rc_black,rc_white);
	}
}

void console_write_color(char *cstr,real_color_t back,real_color_t fore)
{
	while(*cstr)
	{
		console_putc_color(*cstr++,back,fore);
	}
}

void console_write_hex(uint32_t n, real_color_t back,real_color_t fore)
{
	//32位数用16进制输出最多8位
	char hex_header[]="0x"; //输出十六进制数的格式0x
	char hex_digits[8];		//存放16进制数的每一位，地位在前，高位在后；
	char result[8];			//存放最终结果，高位在前，低位在后
	uint8_t num=0;			//表示该数有几位（16进制）
	
	console_write_color(hex_header,back,fore);

	if(n==0)				//如果是0直接输出0
	{
		console_putc_color('0',back,fore);
		return;
	}

	while(n)				//循环，每次去除最低位放在hex_digits里边，并且让n除以16
	{
		char digit=n & (0xf);
		hex_digits[num++]=hex_set[digit];
		n=n>>4;
	}
	uint8_t i,j;
	for(i=0,j=num-1;i<num;i++,j--)
	{
		result[i]=hex_digits[j];
	}
	result[num]=0;
	console_write_color(result,back,fore);
}

void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
	//32位数用10进制输出最多10位
	char dec_digits[10];		//存放10进制数的每一位，低位在前，高位在后；
	char result[10];			//存放最终结果，高位在前，低位在后
	uint8_t num=0;				//表示该数有几位（10进制）

	if(n==0)				//如果是0直接输出0
	{
		console_putc_color('0',back,fore);
		return;
	}

	while(n)				//循环，每次去除最低位放在dec_digits里边，并且让n除以10
	{
		char digit=n%10;
		dec_digits[num++]=dec_set[digit];
		n/=10;
	}
	uint8_t i,j;
	for(i=0,j=num-1;i<num;i++,j--)
	{
		result[i]=dec_digits[j];
	}
	result[num]=0;
	console_write_color(result,back,fore);
}