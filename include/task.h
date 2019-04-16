/*
 * =====================================================================================
 *
 *       Filename:  task.h
 *
 *    Description:  任务（线程）相关定义
 *
 *        Version:  1.0
 *        Created:  2019.4.12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TASK_H
#define INCLUDE_TASK_H

#include "types.h"
#include "pmm.h"
#include "vmm.h"

// 进程状态描述
typedef
enum task_state {
    TASK_UINIT = 0,     //未初始化
    TASK_SLEEPING = 1,  //睡眠中
    TASK_RUNNABLE = 2,  //可运行或正在运行
    TASK_ZOMBIE = 3,    //僵尸状态
} task_state;

// 内核线程的上下文切换保存的信息，寄存器
struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
};

// 进程内存地址结构
struct mm_struct {
    pgd_t *pgd_dir;     //进程页表
};

// 进程控制块PCB
struct task_struct {
    volatile task_state state;      // 进程当前状态
    pid_t pid;                      // 进程标识符
    void *stack;                    // 进程的内核栈地址
    struct mm_struct *mm;           // 当前进程的内存地址映像
    struct context context;         // 进程切换需要的上下文信息
    struct task_struct *next;       // 链表指针
};

// 全局pid值
extern pid_t now_pid;

// 内核线程创建
int32_t kernel_thread(int (*fn)(void *),void *arg);

// 线程退出函数
void kthread_exit();

#endif  // INCLUDE_TASK_H