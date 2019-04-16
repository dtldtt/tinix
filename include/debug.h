/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  调试相关的声明
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
#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

#include "console.h"
#include "vargs.h"
#include "elf.h"

#define assert(x,info)      \
    do {                    \
        if (!(x)) {         \
            panic(info);    \
        }                   \
    } while(0)

//编译期间静态检测
#define static_assert(x)    \
    switch (x) { case:0 case (x):;}

// 初始化Debug信息
void init_debug();

// 打印当前函数的调用栈信息
void panic(const char *msg);

// 打印当前的段存器值
void print_cur_status();

//内核的打印函数
void printk(const char *format, ...);

//带颜色的内核打印函数
void printk_color(real_color_t back, real_color_t fore, const char *format, ...);

#endif  //INCLUDE_DEBUG_H