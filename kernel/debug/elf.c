/*
 * =====================================================================================
 *
 *       Filename:  elf.c
 *
 *    Description:  ELF文件格式相关实现
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

 #include "common.h"
 #include "string.h"
 #include "elf.h"
 //#include "vmm.h"

 //从multiboot_t 结构中获取信息ELF
 elf_t elf_from_multiboot(section_header_t *sht)
 {
     int i;
     elf_t elf;
     elf_section_header_t *sh = (elf_section_header_t *)sht->addr;

     uint32_t shstrtab = sh[sht->shndx].addr;
     for (i=0;i<sht->num;i++) {      //遍历所有节section
         const char *name=(const char *)(shstrtab + sh[i].name);
         //printk("%s\n",name);
         //在GRUB提供的multiboot信息中找内核ELF格式所提取的字符串表和符号表
         if(strcmp(name,".strtab")==0) {
             //printk("%s\n",name);
             elf.strtab=(const char *)sh[i].addr ;
             elf.strtabsz=sh[i].size;
         }
         if(strcmp(name,".symtab")==0){
             //printk("%s\n",name);
             elf.symtab=(elf_symbol_t *)sh[i].addr ;
             elf.symtabsz=sh[i].size;
         }
     }
     return elf;
 }

 //查看的符号信息ELF
 const char* elf_lookup_symbol(uint32_t addr, elf_t *elf)
 {
     int i;

     for (i=0;i<(elf->symtabsz/sizeof(elf_symbol_t));i++)
     {
         if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2) {
             continue;
         }
         // 通过函数调用地址查到函数的名字
         if ( (addr>= elf->symtab[i].value) && (addr<(elf->symtab[i].value +elf->symtab[i].size))) {
             return (const char *) ((uint32_t)elf->strtab+elf->symtab[i].name);
         }
     }
     return NULL;
 }