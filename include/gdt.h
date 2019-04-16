/*
 * =====================================================================================
 *
 *       Filename:  gdt.h
 *
 *    Description:  gdt设置相关声明
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

 #ifndef INCLUDE_GDT_H
 #define INCLUDE_GDT_H

 #include "types.h"

// 全局描述符类型，参考GDT表项内容
typedef
struct gdt_entry_t {
    uint16_t limit_low;     //段界限 0~15
    uint16_t base_low;      //段基址 0~15
    uint8_t base_middle;    //段基址 16~23
    uint8_t access;         //段存在位P，描述符特权级DPL，描述符类型S，描述符子类别TYPE
    uint8_t granularity;    //粒度G，操作数大小D/B，64位标志L，软件可用位AVL，段界限16~19
    uint8_t base_high;      //段基址24~31
} __attribute__((packed)) gdt_entry_t;

// GDTR
typedef
struct gdt_ptr_t {
    uint16_t limit;     //全局描述符表限长
    uint32_t base;      //全局描述符表基址
} __attribute__((packed)) gdt_ptr_t;

// 初始化全局描述符表GDTR
void init_gdt();

// GDT 加载到GDTR的汇编实现函数
extern void gdt_flush(uint32_t);

#endif      //INCLUDE_GDT_H