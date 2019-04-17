// /*
//  * =====================================================================================
//  *
//  *       Filename:  sched.c
//  *
//  *    Description:  调度机制相关实现
//  *
//  *        Version:  1.0
//  *        Created:  2019.4.12
//  *       Revision:  none
//  *       Compiler:  gcc
//  *
//  *         Author:  DTL(Tongliang Dong)
//  *    
//  *
//  * =====================================================================================
//  */

// #include "sched.h"
// #include "heap.h"
// #include "debug.h"
// #include "pmm.h"

// // 可调度进程链表
// struct task_struct *running_proc_head = NULL;

// // 等待进程链表
// struct task_struct *wait_proc_head = NULL;

// // 当前运行的任务
// struct task_struct *current = NULL;

// void init_sched()
// {
//     // 为当前正在执行的任务链表创建结构体，位于栈的最低端
//     current = (struct task_struct*)(kern_stack_top - STACK_SIZE);

//     current->state = TASK_RUNNABLE;
//     current->pid = now_pid++;
//     current->stack = current;
//     current->mm = NULL;     //内核线程不需要，内核线程共享内存空间

//     // 单项循环链表
//     current->next = current;
//     running_proc_head = current;
// }

// void schedule()
// {
//     if(current) {
//         change_task_to(current->next);
//     }
// }

// void change_task_to(struct task_struct *next)
// {
//     if(current != next) {
//         struct task_struct *prev = current;
//         current = next;
//         switch_to(&(prev->context),&(current->context));
//     }
// }

