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

section .init.text

[EXTERN kern_entry]
[GLOBAL section_header_table]


    mov [section_header_table],ebx

    call kern_entry

stop:
    hlt
    jmp stop
section_header_table:
    resb 4