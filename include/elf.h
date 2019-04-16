/*
 * =====================================================================================
 *
 *       Filename:  elf.h
 *
 *    Description:  ELF文件格式相关声明
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
#ifndef INCLUDE_ELF_H
#define INCLUDE_ELF_H

#include "types.h"
#include "multiboot.h"

#define ELF32_ST_TYPE(i) ((i)&0xf)

//ELF 格式区段头
typedef
struct elf_section_header_t 
{
    uint32_t name;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addralign;
    uint32_t entsize;
} __attribute__((packed)) elf_section_header_t;

// ELF 格式符号
typedef
struct elf_symbol_t 
{
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t ohter;
    uint16_t shndx;
} __attribute__((packed)) elf_symbol_t;

// ELF信息
typedef
struct elf_t 
{
    elf_symbol_t *symtab;
    uint32_t symtabsz;
    const char *strtab;
    uint32_t strtabsz;
} elf_t;

//从multiboot_t结构获取信息ELF
elf_t elf_from_multiboot(multiboot_t *mb);

//查看的符号信息ELF
const char* elf_lookup_symbol(uint32_t addr,elf_t *elf);

#endif      // INCLUDE_ELF_H