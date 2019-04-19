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

extern section_header_t *section_header_table;

#endif