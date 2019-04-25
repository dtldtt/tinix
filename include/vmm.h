/*
 * =====================================================================================
 *
 *       Filename:  vmm.h
 *
 *    Description:  虚拟内存管理的定义virtual memory management
 *
 *        Version:  1.0
 *        Created:  2019.4.02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */
#ifndef INCLUDE_VMM_H
#define INCLUDE_VMM_H

#include "types.h"
#include "idt.h"
#include "init.h"
//#include "vmm.h"

// 页目录在内核中的地址，在loader.S中设置了
#define PGD_ADDR 0xc0200000

// 第一个页表在内核中的地址
#define PTE_ADDR 0xc0201000

// 内核的偏移地址
#define PAGE_OFFSET 0xc0000000

// 下面介绍一下页表和页目录项的其中几位代表的意思
/**
 * P---第0位 存在位
 * 为0时，表示页表或页不在内存中，必须先行调入
 * 为1时，表示页表或页位于内存中
 * 这里设置为1
 */
#define PAGE_PRESENT 0x1

/**
 * R/W位---第1位，读/写标志位
 * 为0只读，为1可读可写
 */
#define PAGE_WRITE 0x2
#define PAGE_READ 0x0

/**
 * U/S---第2位，用户/超级用户位
 * 为1时，表示任何特权级程序都可以访问该页面
 * 为0时，表示只能被运行在特权机（0,1,2）上的程序访问
 */
#define PAGE_USER 0x4
#define PAGE_SYS 0x0

// 虚拟分页大小
#define PAGE_SIZE   4096

// 也掩码，用于4KB对齐
#define PAGE_MASK   0xfffff000

/**
 * 以下三个宏用于把一个虚拟地址分为三部分单独取出来
 * 具体是页目录，页表，页内偏移三部分
 * 分别是高10位，中间10位和低12位
 * 具体参见“x86分页机制”文档
 */

// 获取一个地址的页目录项
#define PGD_INDEX(x) (((x)>>22) & 0x3ff)

// 获取一个地址的页表项
#define PTE_INDEX(x) (((x)>>12) & 0x3ff)

// 获取一个地址的页内偏移
#define OFFSET_INDEX(x) ((x) && 0xfff)

// 页目录数据类型
typedef uint32_t pgd_t;

// 页表数据类型
typedef uint32_t pte_t;

// 一个页内页目录存放的页表成员数
#define PGD_SIZE (PAGE_SIZE/sizeof(pte_t))

// 一个页内页表存放的页成员数
#define PTE_SIZE (PAGE_SIZE/sizeof(uint32_t))

// 映射512MB 内存所需要的页表数
// 一个页表最多可以存储4096/4=1024个页项
// 512MB/4096/1024=128个页表
#define PTE_COUNT 128

// 内核页目录区域
extern pgd_t *pgd_kern;

// 初始化虚拟内存管理
void init_vmm();

// 更换当前的页目录
void switch_pgd(uint32_t pd);

// 使用flags指出的页权限，把虚拟地址va映射到物理地址pa
void map(pgd_t *pgd_now, uint32_t va,uint32_t pa,uint32_t flags);

// 取消虚拟地址va的物理映射
void unmap(pgd_t *pgd_now,uint32_t va);

// 如果虚拟地址va映射到物理地址则返回1
// 同时如果pa不是空指针则把va对应的物理地址写入到pa
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// 页错误中断的中断处理函数
void page_fault(pt_regs_t *regs);

#endif  // INCLUDE_VMM_H