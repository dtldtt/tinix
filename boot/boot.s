; ----------------------------------------------------------------
;
; 	boot.s -- 内核从这里开始
;
;                 这里还有根据 GRUB Multiboot 规范的一些定义
;
; ----------------------------------------------------------------

; MBOOT_HEADER_MAGIC 	equ 	0x1BADB002 	; Multiboot 魔数，由规范决定的

; MBOOT_PAGE_ALIGN 	equ 	1 << 0    	; 0 号位表示所有的引导模块将按页(4KB)边界对齐
; MBOOT_MEM_INFO 		equ 	1 << 1    	; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息
; 						; (告诉GRUB把内存空间的信息包含在Multiboot信息结构中)

; ; 定义我们使用的 Multiboot 的标记
; MBOOT_HEADER_FLAGS 	equ 	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; ; 域checksum是一个32位的无符号值，当与其他的magic域(也就是magic和flags)相加时，
; ; 要求其结果必须是32位的无符号值 0 (即magic + flags + checksum = 0)
; MBOOT_CHECKSUM 		equ 	- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

; ; 符合Multiboot规范的 OS 映象需要这样一个 magic Multiboot 头

; ; Multiboot 头的分布必须如下表所示：
; ; ----------------------------------------------------------
; ; 偏移量  类型  域名        备注
; ;
; ;   0     u32   magic       必需
; ;   4     u32   flags       必需 
; ;   8     u32   checksum    必需 
; ;
; ; 我们只使用到这些就够了，更多的详细说明请参阅 GNU 相关文档
; ;-----------------------------------------------------------

; ;-----------------------------------------------------------------------------

; [BITS 32]  	; 所有代码以 32-bit 的方式编译

; section .init.text 	; 代码段从这里开始

; ; 在代码段的起始位置设置符合 Multiboot 规范的标记

; dd MBOOT_HEADER_MAGIC 	; GRUB 会通过这个魔数判断该映像是否支持
; dd MBOOT_HEADER_FLAGS   ; GRUB 的一些加载时选项，其详细注释在定义处
; dd MBOOT_CHECKSUM       ; 检测数值，其含义在定义处

; [GLOBAL start] 		; 内核代码入口，此处提供该声明给 ld 链接器
; [GLOBAL mboot_ptr_tmp] 	; 全局的 struct multiboot * 变量
; [EXTERN kern_entry] 	; 声明内核 C 代码的入口函数

; start:
; 	cli  			 ; 此时还没有设置好保护模式的中断处理，要关闭中断
; 				 ; 所以必须关闭中断
; 	;mov esp, STACK_TOP  	 ; 设置内核栈地址
; 	;mov ebp, 0 		 ; 帧指针修改为 0
; 	;and esp, 0FFFFFFF0H	 ; 栈地址按照16字节对齐
; 	;mov [glb_mboot_ptr], ebx ; 将 ebx 中存储的指针存入全局变量
; 	;call kern_entry		 ; 调用内核入口函数
; 	mov [mboot_ptr_tmp],ebx
; 	mov esp, STACK_TOP
; 	and esp,0xfffffff0
; 	mov ebp,0

; 	call kern_entry
; ;stop:
; 	;hlt 			 ; 停机指令，什么也不做，可以降低 CPU 功耗
; 	;jmp stop 		 ; 到这里结束，关机什么的后面再说

; ;-----------------------------------------------------------------------------
; section .init.data
; ;section .bss 			 ; 未初始化的数据段从这里开始
; stack:
; 	;resb 32768 	 	 ; 这里作为内核栈
; 	times 1024 db 0
; ;glb_mboot_ptr: 			 ; 全局的 multiboot 结构体指针
; ;	resb 4

; STACK_TOP equ $-stack-1 	 ; 内核栈顶，$ 符指代是当前地址
; mboot_ptr_tmp: dd 0
; ;-----------------------------------------------------------------------------

%include "boot.inc"



SECTION .text vstart=0x7c00
[GLOBAL start]       
start:  
   mov ax,cs      
   mov ds,ax
   mov es,ax
   mov ss,ax
   mov fs,ax
   mov sp,0x7c00
   mov ax,0xb800
   mov gs,ax

; 清屏
;利用0x06号功能，上卷全部行，则可清屏。
; -----------------------------------------------------------
;INT 0x10   功能号:0x06	   功能描述:上卷窗口
;------------------------------------------------------
;输入：
;AH 功能号= 0x06
;AL = 上卷的行数(如果为0,表示全部)
;BH = 上卷行属性
;(CL,CH) = 窗口左上角的(X,Y)位置
;(DL,DH) = 窗口右下角的(X,Y)位置
;无返回值：
   mov     ax, 0600h
   mov     bx, 0700h
   mov     cx, 0                   ; 左上角: (0, 0)
   mov     dx, 184fh		   ; 右下角: (80,25),
				   ; 因为VGA文本模式中，一行只能容纳80个字符,共25行。
				   ; 下标从0开始，所以0x18=24,0x4f=79
   int     10h                     ; int 10h

   ; 输出字符串:MBR
   mov byte [gs:0x00],'1'
   ;mov byte [gs:0x01],0xA4

   mov byte [gs:0x02],' '
   ;mov byte [gs:0x03],0xA4

   mov byte [gs:0x04],'M'
   ;mov byte [gs:0x05],0xA4	   ;A表示绿色背景闪烁，4表示前景色为红色

   mov byte [gs:0x06],'B'
   ;mov byte [gs:0x07],0xA4

   mov byte [gs:0x08],'R'
   ;mov byte [gs:0x09],0xA4

;    ; 接下来移动自身到0x90000处，这样可以修改段寄存器，将loader移动到想要的0x9d000
;    ; 这个完全可以当成boot最开始的代码，前边的其实只是为了测试代码是正常的，所以没有去掉了。
; copy_myself:
;    mov ax,BOOTSEC
;    mov ds,ax
;    mov ax,INITSEC
;    mov es,ax
;    mov cx,256		;一次移动一个字，总共512字节，共需要256次
;    xor si,si
;    xor di,di
;    rep movsw

; 	; 跳转到复制后地方的boot执行
; 	jmp INITSEC:go

; 	;准备读入loader到0x9d000处
; go:
; 	mov ax,INITSEC
; 	mov ds,ax	 
   mov eax,LOADER_START_SECTOR	 ; 起始扇区lba地址
   mov bx,LOADER_BASE_ADDR       ; 写入的地址
   mov cx,4			 ; 待读入的扇区数
   call rd_disk_m_16		 ; 以下读取程序的起始部分（一个扇区）
  
;    jmp far [jmp_to_loader]

; jmp_to_loader:
; 	dw 0x300,
; 	dw LOADER_BASE_ADDR/16
	jmp LOADER_BASE_ADDR + 0x300
       
;-------------------------------------------------------------------------------
;功能:读取硬盘n个扇区
rd_disk_m_16:	   
;-------------------------------------------------------------------------------
				       ; eax=LBA扇区号
				       ; ebx=将数据写入的内存地址
				       ; ecx=读入的扇区数
      mov esi,eax	  ;备份eax
      mov di,cx		  ;备份cx
;读写硬盘:
;第1步：设置要读取的扇区数
      mov dx,0x1f2
      mov al,cl
      out dx,al            ;读取的扇区数

      mov eax,esi	   ;恢复ax

;第2步：将LBA地址存入0x1f3 ~ 0x1f6

      ;LBA地址7~0位写入端口0x1f3
      mov dx,0x1f3                       
      out dx,al                          

      ;LBA地址15~8位写入端口0x1f4
      mov cl,8
      shr eax,cl
      mov dx,0x1f4
      out dx,al

      ;LBA地址23~16位写入端口0x1f5
      shr eax,cl
      mov dx,0x1f5
      out dx,al

      shr eax,cl
      and al,0x0f	   ;lba第24~27位
      or al,0xe0	   ; 设置7～4位为1110,表示lba模式
      mov dx,0x1f6
      out dx,al

;第3步：向0x1f7端口写入读命令，0x20 
      mov dx,0x1f7
      mov al,0x20                        
      out dx,al

;第4步：检测硬盘状态
  .not_ready:
      ;同一端口，写时表示写入命令字，读时表示读入硬盘状态
      nop
      in al,dx
      and al,0x88	   ;第4位为1表示硬盘控制器已准备好数据传输，第7位为1表示硬盘忙
      cmp al,0x08
      jnz .not_ready	   ;若未准备好，继续等。

;第5步：从0x1f0端口读数据
      mov ax, di
      mov dx, 256
      mul dx
      mov cx, ax	   ; di为要读取的扇区数，一个扇区有512字节，每次读入一个字，
			   ; 共需di*512/2次，所以di*256
      mov dx, 0x1f0
  .go_on_read:
      in ax,dx
      mov [bx],ax
      add bx,2		  
      loop .go_on_read
      ret

   times 510-($-$$) db 0
   db 0x55,0xaa
