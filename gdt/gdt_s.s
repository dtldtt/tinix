; ;/*
; ; * =====================================================================================
; ; *
; ; *       Filename:  gdt_s.s
; ; *
; ; *    Description:  把GDT载入GDTR汇编实现
; ; *
; ; *        Version:  1.0
; ; *        Created:  2019.3.24
; ; *       Revision:  none
; ; *       Compiler:  gcc
; ; *
; ; *         Author:  DTL(Tongliang Dong)
; ; *    
; ; *
; ; * =====================================================================================
; ; */

; [GLOBAL gdt_flush]

; gdt_flush:
;     mov eax, [esp+4]    ;参数载入eax寄存器
;     lgdt [eax]          ;装在GDTR

;     ;段选择子格式自查，第3位开始表示描述符编号.
;     mov ax,0x10         ;2号段描述符，即数据段描述符
;     mov ds,ax           ;更新段寄存器
;     mov es,ax
;     mov fs,ax
;     mov gs,ax
;     mov ss,ax
;     jmp 0x08:.flush     ;远跳转，0x08是段描述符，提供给CS段选择子，即第1个段描述符，代码段
;                         ;目的是在进入保护模式猴清空流水线并串行化处理器

; .flush:
;     ret