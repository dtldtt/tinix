/*
 * =====================================================================================
 *
 *       Filename:  init.h
 *
 *    Description:  内核初始化声明
 *
 *        Version:  1.0
 *        Created:  2019.4.23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#ifndef INCLUDE_INIT_H
#define INCLUDE_INIT_H

#include "types.h"
#include "console.h"
#include "debug.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "sched.h"
#include "task.h"

// 内核栈
uint32_t kern_stack;
// 内核栈顶
uint32_t kern_stack_top;

// 初始化内核
void init_all();

#endif