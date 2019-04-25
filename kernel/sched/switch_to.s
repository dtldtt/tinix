[global switch_to]

; 具体的线程切换操作，主要是寄存器的保存和恢复
switch_to:
    mov eax,[esp+4] ;对应prev->context，接下来保存原进程相关寄存器到栈
                    ;按照struct context里的顺序
    mov [eax],esp
    mov [eax+4],ebp
    mov [eax+8],ebx
    mov [eax+12],esi
    mov [eax+16],edi

    pushf           ; 压栈表示寄存器，再取出，保存到栈里
    pop ecx
    mov [eax+20],ecx

    mov eax,[esp+8] ; 对应current->context,接下来恢复current进程的寄存器

    mov esp,[eax]
    mov ebp,[eax+4]
    mov ebx,[eax+8]
    mov esi,[eax+12]
    mov edi,[eax+16]
    mov eax,[eax+20] ; 标志寄存器内容，先放到eax
    push eax
    popf             ; 出栈标志寄存器

    ret             ;恢复到调用函数下一句话执行，注意此时任务栈已经切换，所以相应的也是该任务下的内核栈的内容
