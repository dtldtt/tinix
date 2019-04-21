/*
 * =====================================================================================
 *
 *       Filename:  gdt.c
 *
 *    Description:  gdt设置相关实现
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

 #include "gdt.h"
 #include "string.h"
 #include "debug.h"
 // 当前gdt表项数
 static int gdt_length=0;

 //GDTR
 gdt_ptr_t gdt_ptr;

 // 根据GDT描述符的格式设置GDT描述符的每一位,num表示第几个表项，其余参见gdt_entry_t定义
 static void gdt_set_gate(int32_t num, uint32_t base,
                        uint32_t limit,uint8_t access, uint8_t gran);

 // 内核的栈地址
 extern uint32_t stack;

 // 初始化全局描述符表
 void init_gdt()
 {
     // 全局描述符表从0开始，所以界限-1
     //printk("%X\n",gdt_entries);
     //printk("%x\n",(uint32_t)gdt_entries);
     gdt_ptr.limit= 0;
     gdt_ptr.base=(uint32_t) gdt_entries;

     // 设置每一个表项
     gdt_set_gate(gdt_length,0,0,0,0);                   // 规定第一个描述符必须全0
     gdt_set_gate(gdt_length,0,0xffffffff,0x9a,0xcf);    // 代码段
     gdt_set_gate(gdt_length,0,0xffffffff,0x92,0xcf);    // 数据段
     //printk("\nafter set gate%X\n",gdt_entries);

     // 加载GDT到GDTR寄存器
     gdt_flush((uint32_t)&gdt_ptr);
     //printk("\nafter flush%X\n",gdt_entries);

 }

 // 全局描述符表构造函数，根据下标num构造
 // 参数分别是数组下标，基地址，限长，访问标志，其它标志，具体见gdt_entry_t 定义
 static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit,uint8_t access,uint8_t gran)
 {
     gdt_entries[num].base_low= base & 0xffff;
     gdt_entries[num].base_middle= (base>>16) & 0xff;
     gdt_entries[num].base_high= (base>>24) & 0xff;

     gdt_entries[num].limit_low= limit & 0xffff;
     gdt_entries[num].granularity= (limit>>16) & 0x0f;

     gdt_entries[num].granularity |= gran & 0xf0;
     gdt_entries[num].access = access;

     gdt_length++;
     gdt_ptr.limit = sizeof(gdt_entry_t) * gdt_length - 1 ;
 }

 void print_gdt()
 {
    
     printk("0x%X\n0x%X\n0x%X\n",&gdt_ptr,gdt_ptr.base,gdt_entries);
    
 }