/*
 * =====================================================================================
 *
 *       Filename:  heap.h
 *
 *    Description:  内存堆管理的定义
 *
 *        Version:  1.0
 *        Created:  2019.4.10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#ifndef INCLUDE_HEAP_H
#define INCLUDE_HEAP_H

#include "types.h"
#include "information.h"

// 堆起始地址，放在内存结束的后20M，留给堆的空间是20M，在初始化时赋值
uint32_t heap_start;

// 内存块的头部
// 每一个分配的内存块都有一个头部，方便管理
typedef
struct header {
    struct header *prev;    // 前一个内存块头部
    struct header *next;    // 后一个内存块头部
    uint32_t allocated : 1; // 表示该内存块是否已经被分配
    uint32_t length : 31;   // 该内存块的长度（算头部）
} header_t;

// 初始化堆
void init_heap();

// 内存申请
void *kmalloc(uint32_t len);

// 内存释放
void kfree(void *p);

// 内核堆的测试函数
void test_heap();

#endif  // INCLUDE_HEAP_H