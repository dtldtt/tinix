/*
 * =====================================================================================
 *
 *       Filename:  keyboard.c
 *
 *    Description:  键盘驱动程序实现
 *
 *        Version:  1.0
 *        Created:  2019.5.04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL (TONGLIANG DONG)
 *
 * =====================================================================================
 */
#include "keyboard.h"
#include "debug.h"
#include "idt.h"
#include "common.h"
#include "string.h"

#define KBD_BUF_PORT 0x60	   // 键盘buffer寄存器端口号为0x60

/* 用转义字符定义部分控制字符 */
// \x开头的表示十六进制的控制转义字符
#define esc		'\x1b'	 // 也可以用十六进制'\x1b'
#define backspace	'\b'
#define tab		'\t'
#define enter		'\n'
#define delete		'\x7f'	 // 十六进制为'\x7f'

/* 以下不可见字符一律定义为0 */
// 包括左右ctrl,shift,alt和大写锁定键
#define char_invisible	0
#define ctrl_l_char	char_invisible
#define ctrl_r_char	char_invisible
#define shift_l_char	char_invisible
#define shift_r_char	char_invisible
#define alt_l_char	char_invisible
#define alt_r_char	char_invisible
#define caps_lock_char	char_invisible

/* 定义控制字符的通码和断码 */
#define shift_l_make	0x2a
#define shift_r_make 	0x36 
#define alt_l_make   	0x38
#define alt_r_make   	0xe038
#define alt_r_break   	0xe0b8
#define ctrl_l_make  	0x1d
#define ctrl_r_make  	0xe01d
#define ctrl_r_break 	0xe09d
#define caps_lock_make 	0x3a

#define BUFFER_SIZE	64
/* 定义以下变量记录相应键是否按下的状态,
 * ext_scancode用于记录makecode是否以0xe0开头 
 * 因为操作控制键都是与其他键配合使用的，而且操作控制键都是先按下的
 * 所以得在全局保存控制键是否被按下
 * 比如我们平时使用的全选ctrl+a，在按下a的时候，要检查ctrl是否被按下，如果按下才是全选的意思，否则就是a字符了
 */ 
static bool ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;
// 键盘输入缓冲区，大小为64
static char keyboard_buffer[BUFFER_SIZE];
// 标识当前缓冲区有几个字符
static int current_pos;

extern char* input_buffer;
extern int input_pos;
/* 以通码make_code为索引的二维数组 具体通码可以在百度搜索“第一套键盘扫描码”*/
//这里我们只处理通码为0-0x3a的按键范围，这已经包括了我们平时使用的绝大多数按键（不包括小键盘）
//数组咦通码为索引，这样就可以找到通码对应的ascii
//由于主键盘区域通常与shift键配合使用，所以这里专门写出了按下shift键的效果，在数组第二列。
static char keymap[][2] = {
/* 扫描码   未与shift组合  与shift组合*/
/* ---------------------------------- */
/* 0x00 */	{0,	0},		
/* 0x01 */	{esc,	esc},		
/* 0x02 */	{'1',	'!'},		
/* 0x03 */	{'2',	'@'},		
/* 0x04 */	{'3',	'#'},		
/* 0x05 */	{'4',	'$'},		
/* 0x06 */	{'5',	'%'},		
/* 0x07 */	{'6',	'^'},		
/* 0x08 */	{'7',	'&'},		
/* 0x09 */	{'8',	'*'},		
/* 0x0A */	{'9',	'('},		
/* 0x0B */	{'0',	')'},		
/* 0x0C */	{'-',	'_'},		
/* 0x0D */	{'=',	'+'},		
/* 0x0E */	{backspace, backspace},	
/* 0x0F */	{tab,	tab},		
/* 0x10 */	{'q',	'Q'},		
/* 0x11 */	{'w',	'W'},		
/* 0x12 */	{'e',	'E'},		
/* 0x13 */	{'r',	'R'},		
/* 0x14 */	{'t',	'T'},		
/* 0x15 */	{'y',	'Y'},		
/* 0x16 */	{'u',	'U'},		
/* 0x17 */	{'i',	'I'},		
/* 0x18 */	{'o',	'O'},		
/* 0x19 */	{'p',	'P'},		
/* 0x1A */	{'[',	'{'},		
/* 0x1B */	{']',	'}'},		
/* 0x1C */	{enter,  enter},
/* 0x1D */	{ctrl_l_char, ctrl_l_char},
/* 0x1E */	{'a',	'A'},		
/* 0x1F */	{'s',	'S'},		
/* 0x20 */	{'d',	'D'},		
/* 0x21 */	{'f',	'F'},		
/* 0x22 */	{'g',	'G'},		
/* 0x23 */	{'h',	'H'},		
/* 0x24 */	{'j',	'J'},		
/* 0x25 */	{'k',	'K'},		
/* 0x26 */	{'l',	'L'},		
/* 0x27 */	{';',	':'},		
/* 0x28 */	{'\'',	'"'},		
/* 0x29 */	{'`',	'~'},		
/* 0x2A */	{shift_l_char, shift_l_char},	
/* 0x2B */	{'\\',	'|'},		
/* 0x2C */	{'z',	'Z'},		
/* 0x2D */	{'x',	'X'},		
/* 0x2E */	{'c',	'C'},		
/* 0x2F */	{'v',	'V'},		
/* 0x30 */	{'b',	'B'},		
/* 0x31 */	{'n',	'N'},		
/* 0x32 */	{'m',	'M'},		
/* 0x33 */	{',',	'<'},		
/* 0x34 */	{'.',	'>'},		
/* 0x35 */	{'/',	'?'},
/* 0x36	*/	{shift_r_char, shift_r_char},	
/* 0x37 */	{'*',	'*'},    	
/* 0x38 */	{alt_l_char, alt_l_char},
/* 0x39 */	{' ',	' '},		
/* 0x3A */	{caps_lock_char, caps_lock_char}
/*其它按键暂不处理*/
};

/* 键盘中断处理程序 */
static void intr_keyboard_handler(pt_regs_t *regs) {

/* 这次中断发生前的上一次中断,以下任意三个键是否有按下 */
	bool ctrl_down_last = ctrl_status;	  
	bool shift_down_last = shift_status;
	bool caps_lock_last = caps_lock_status;

	bool break_code;
	uint16_t scancode = inb(KBD_BUF_PORT);

/* 若扫描码是e0开头的,表示此键的按下将产生多个扫描码,
 * 所以马上结束此次中断处理函数,等待下一个扫描码进来*/ 
	if (scancode == 0xe0) { 
	   ext_scancode = TRUE;    // 打开e0标记
	   return;
	}

/* 如果上次是以0xe0开头,将扫描码合并 */
	if (ext_scancode) {
	   scancode = ((0xe000) | scancode);
	   ext_scancode = FALSE;   // 关闭e0标记
	}   
// 这里用来判断获取到的键盘扫描码是通码还是断码，由于大多数情况下断码=通码+0x80
// 所以断码的第7位是1，而通码为0，所以通过判断第7位是否为0来确定是通码还是断码
	break_code = ((scancode & 0x0080) != 0);   // break_code为0是通码，为1是断码
   
	if (break_code) {   // 若是断码break_code(按键弹起时产生的扫描码)

   /* 由于ctrl_r 和alt_r的make_code和break_code都是两字节,
   所以可用下面的方法取make_code,多字节的扫描码暂不处理 */
		uint16_t make_code = (scancode &= 0xff7f);   // 得到其make_code(按键按下时产生的扫描码)，相当于减0x80

	/* 若是任意以下三个键弹起了,将状态置为FALSE */
		if (make_code == ctrl_l_make || make_code == ctrl_r_make) {
			ctrl_status = FALSE;
		} else if (make_code == shift_l_make || make_code == shift_r_make) {
			shift_status = FALSE;
		} else if (make_code == alt_l_make || make_code == alt_r_make) {
			alt_status = FALSE;
		} /* 由于caps_lock不是弹起后关闭,所以需要单独处理 */

		return;   // 直接返回结束此次中断处理程序

	} 
   /* 若为通码,只处理数组中定义的键以及alt_right和ctrl键,全是make_code 
    * 之所以要单独处理右边的alt和ctrl键，可以查看上边的宏定义，这两个键的通码有e0前缀
	* 所以超过了我们数组的访问范围0-0x3a，所以要对这两个键单独处理
	*/
	else if ((scancode > 0x00 && scancode < 0x3b) || \
	       (scancode == alt_r_make) || \
	       (scancode == ctrl_r_make)) 
	{
		bool shift = FALSE;  // 判断是否与shift组合,用来在一维数组中索引对应的字符
	   /* 这里先判断双字符键，就是按下shift和不按下是不一样的，有以下这些键
	   ***** 双字符键 ********
		     0x0e 数字'0'~'9',字符'-',字符'='
		     0x29 字符'`'
		     0x1a 字符'['
		     0x1b 字符']'
		     0x2b 字符'\\'
		     0x27 字符';'
		     0x28 字符'\''
		     0x33 字符','
		     0x34 字符'.'
		     0x35 字符'/' 
	    *******************************/
		if ((scancode < 0x0e) || (scancode == 0x29) || \
		(scancode == 0x1a) || (scancode == 0x1b) || \
		(scancode == 0x2b) || (scancode == 0x27) || \
		(scancode == 0x28) || (scancode == 0x33) || \
		(scancode == 0x34) || (scancode == 0x35)) 
		{  
			if (shift_down_last) {  // 如果同时按下了shift键
				shift = TRUE;
			}
		} 
		// 下面处理字母键，主键盘区域除了双字符键就是字母键
		else 
		{	  // 默认为字母键，shift和大写锁定同时有效还是输出小写字母
			if (shift_down_last && caps_lock_last) {  // 如果shift和capslock同时按下
				shift = FALSE;
			} else if (shift_down_last || caps_lock_last) { // 如果shift和capslock任意被按下
				shift = TRUE;
			} else {
				shift = FALSE;
			}
		}
		uint8_t index = (scancode &= 0x00ff);  // 将扫描码的高字节置0,主要是针对高字节是e0的扫描码.
		char cur_char = keymap[index][shift];  // 在数组中找到对应的字符

		/* 只处理ascii码不为0的键，除去部分不可见的控制字符 */
		if (cur_char) {
			// 处理回车键
			if(cur_char == enter){
				int i;
				printk("%c",enter);
				keyboard_buffer[current_pos++]=enter;
				for(int i=0;i<current_pos;i++){
					input_buffer[input_pos++]=keyboard_buffer[i];
					keyboard_buffer[i]=0;
				}
				current_pos=0;
			}
			// 处理退格键，先退格后删除，从而实现我们平时所用的退格键功能
			else if(cur_char == backspace){
				printk("%c",backspace);
				printk(" ");
				printk("%c",backspace);
				// 退格键除了要删掉屏幕上的，还要把缓冲区的也删掉，保证同步
				current_pos--;
			}
			// 处理其他字符
			else {
				printk("%c",cur_char);
				keyboard_buffer[current_pos++]=cur_char;
				
			}
			return;
		}
		// 如果cur_char为0，那就说明是ctrl,shift,alt或者capslock其中之一被按下了
		/* 记录本次是否按下了下面几类控制键之一,供下次键入时判断组合键 */
		if (scancode == ctrl_l_make || scancode == ctrl_r_make) {
			ctrl_status = TRUE;
		} else if (scancode == shift_l_make || scancode == shift_r_make) {
			shift_status = TRUE;
		} else if (scancode == alt_l_make || scancode == alt_r_make) {
			alt_status = TRUE;
		} else if (scancode == caps_lock_make) {
			/* 不管之前是否有按下caps_lock键,当再次按下时则状态取反,
			* 即:已经开启时,再按下同样的键是关闭。关闭时按下表示开启。*/
			caps_lock_status = !caps_lock_status;
		}
	} 
	else 
	{
		// 剩下的键不予处理，统一输出unknown key
		printk("unknown key\n");
	}
}

/* 键盘初始化 */
void init_keyboard() {
	//测试键盘，通过设置8259A芯片，只打开键盘中断，其他全部关闭
	//主片端口0x21，从片端口0xa1，具体见idt.h
	// outb(0x21,0xfd);
	// outb(0xa1,0xff);
	bzero(keyboard_buffer,BUFFER_SIZE);
	current_pos=0;
	printk("keyboard init start\n");
	//注册键盘中断处理程序
	register_interrupt_handler(IRQ1, intr_keyboard_handler);
	printk("keyboard init done\n");
}

void getline(char *str)
{
	while(1)
	{
		if(input_buffer[input_pos-1]=='\n'){
			break;
		}
	}
	input_buffer[input_pos-1]=0;
	int i=0;
	while(input_buffer[i]){
		str[i]=input_buffer[i];
		input_buffer[i]=0;
		i++;
	}
	str[i]=0;
	input_pos=0;
}

