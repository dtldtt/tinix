; /*
;  * =====================================================================================
;  *
;  *       Filename:  head.s
;  *
;  *    Description:  做一些全局变量的声明，把内核需要的内容通过这个文件保存下来，方便之后内核C语言使用
;  *
;  *        Version:  1.0
;  *        Created:  2019.4.18
;  *       Revision:  none
;  *       Compiler:  gcc
;  *
;  *         Author:  DTL(Tongliang Dong)
;  *    
;  *
;  * =====================================================================================
;  */
[BITS 32]
KERNEL_BASE_BIN_ADDR equ 0x10000
[EXTERN kern_entry]
[GLOBAL section_header_table]
[GLOBAL gdt_entries]
[GLOBAL idt_entries]
[GLOBAL total_mem_bytes]
[GLOBAL mmap_information]

section .data 
; GDT表位置，在.data节的开始
GDT_BASE:   dd  0x00000000 
	        dd  0x00000000

CODE_DESC:  
            dd  0x11111111
	        dd  0x00000000
DATA_DESC:
            dd  0x00000000
            dd  0x00000000

   times 80 dq 0					 ; 此处预留60个四字空位，因为每个描述符占4个字（8字节） 

; IDT表的位置，紧挨在GDT后边，这里显式定义一个IDT_BASE作为IDT入口的符号，后边定义255个八个字节，因为最多有256个中断
IDT_BASE:   dd  0x00000000
            dd  0x00000000
    
    times 255 dq 0

; 把loader中的section_header_table的内容转移过来，防止以后loader所在内存被内核重复使用，数据丢失
section_header_table_transfer:
            times 4 dd 0

; 全局变量section_header_table ,用来解析内核elf文件头的节头
section_header_table:
            dd section_header_table_transfer

; GDT在内存中的位置存放在全局变量gdt_entries中，在内核函数中用到
gdt_entries:
            dd GDT_BASE
; IDT在内存中的位置存放在全局变量idt_entries中，在内核中用到
idt_entries:
            dd IDT_BASE
; 内存容量存放在全局变量total_mem_bytes中，在内核中用到
total_mem_bytes:
            dd 0
; 把ards_addr的内容从loader转移到内核的head中，因为在后续内核的执行过程中
; boot和loader所在的内存区域可能会被重新使用,所以我们要把这种全局都有可能使用的数据放在内核的.data节中
ards_addr:
            times 320 db 0 
; 内存的相关信息存放在全局变量中mmap_info中
ards_info:
            dd 0                ; 这个双字部分存放ards_num
            dd ards_addr
mmap_information:
            dd ards_info




section .init.text



    mov [total_mem_bytes],eax
    mov [ards_info],ecx
    push ecx

    ; 转移section_header_table到内核所在内存区域，这个结构体只有4个4字节的数据，所以用movsd转移四次就可以了
    mov ecx,4
    mov esi,ebx
    mov edi,section_header_table_transfer
    cld
    rep movsd
    
    ; 下面指令用来吧ards的所有结构体从loader地址空间中搬过来，ecx中最终保留要搬运的次数，因为使用了movsb
    ; 每次搬运1个字节，而一个结构体中是20个字节，总共需要搬运20*ards_num个字节，这个数据最后存放在ecx中
    ; 原地址自然是从edx传送过来的在loader中ards_addr的地址，目的地址是本程序中ards_addr的地址
    xor eax,eax
    pop ecx
    push edx
    mov ax,20
    mul cx
    mov ecx,eax
    pop edx
    mov esi,edx
    mov edi,ards_addr
    cld
    rep movsb


    call kern_entry

stop:
    hlt
    jmp stop
