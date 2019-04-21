/*
 * =====================================================================================
 *
 *       Filename:  multiboot.h
 *
 *    Description:  GRUB获取的硬件信息的相关声明
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
#ifndef INCLUDE_MULTIBOOT_H
#define INCLUDE_MULTIBOOT_H

#include "types.h"

typedef
struct multiboot_t
{
    uint32_t flags;     //Multiboo的版本信息
    /**
     * 从BIOS得知的可用内存
     * 
     */
    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;

    /**
     * ELF格式内核映像的section头表，包括每项的大小
     * 共有几项以及作为名字索引的字符串
     */
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;

    /**
     *  以下两项是由BIOS提供的内存分布的缓冲区的地址和长度
     *  mmap_addr 是缓冲区的地址，mmap_length是缓冲区的总大小
     *  缓冲区由一个或者多个mmap_entry_t组成
     */
    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed)) multiboot_t;

/**
 * GRUB将内存探测的结果按每个分段整理为mmap_entry结构体的数组。
 * mmap_addr是这个结构体数组的首地址，
 * mmap_length是整个数组的长度。
 */
/**
 * size 是相关结构的大小，单位是字节，它可能大于最小值 20
 * base_addr_low 是启动地址的低位，32base_addr_high 是高 32 位，启动地址总共有 64 位
 * length_low 是内存区域大小的低位，32length_high 是内存区域大小的高 32 位，总共是 64 位
 * type 是相应地址区间的类型，1 代表可用，所有其它的值代表保留区域 RAM
 */
typedef struct mmap_entry_t 
{
    uint32_t size;              // size 是不含size自身变量的大小
    uint32_t base_addr_low;     // 分为两段为以后考虑，不过暂时只考虑32位地址线
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;              // type描述内存段的属性，是指向RAM还是其他外设
} __attribute__((packed)) mmap_entry_t;

#endif      //INCLUDE_MULTIBOOT_H

