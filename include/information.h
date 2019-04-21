/*
 * =====================================================================================
 *
 *       Filename:  information.h
 *
 *    Description:  加载内核的过程中获取的内核和计算机的相关信息
 *
 *        Version:  1.0
 *        Created:  2019.4.17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DTL(Tongliang Dong)
 *    
 *
 * =====================================================================================
 */

#ifndef INCLUDE_INFORMATION_H
#define INCLUDE_INFORMATION_H

#include "types.h"

typedef 
struct section_header_t {
    uint32_t addr;
    uint32_t size;
    uint32_t num;
    uint32_t shndx;
} __attribute__((packed)) section_header_t;

// 定义由head.s提供的内存信息结构
typedef
struct mmap_info {
    /**
     *  以下两项是由BIOS提供的内存分布的缓冲区的地址和长度
     *  mmap_addr 是缓冲区的地址，mmap_length是缓冲区的总大小
     *  缓冲区由一个或者多个mmap_entry_t组成
     */
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) mmap_info;

/**
 * size 是相关结构的大小，单位是字节，它可能大于最小值 20
 * base_addr_low 是启动地址的低位，32base_addr_high 是高 32 位，启动地址总共有 64 位
 * length_low 是内存区域大小的低位，32length_high 是内存区域大小的高 32 位，总共是 64 位
 * type 是相应地址区间的类型，1 代表可用，所有其它的值代表保留区域 RAM
 * 我们这里只考虑32位地址，因为是32位机
 */
typedef struct mmap_entry_t 
{
    //uint32_t size;              // size 是不含size自身变量的大小
    uint32_t base_addr_low;     // 分为两段为以后考虑，不过暂时只考虑32位地址线
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;              // type描述内存段的属性，是指向RAM还是其他外设
} __attribute__((packed)) mmap_entry_t;

extern section_header_t *section_header_table;
extern mmap_info *mmap_information;
extern uint32_t total_mem_bytes;

#endif