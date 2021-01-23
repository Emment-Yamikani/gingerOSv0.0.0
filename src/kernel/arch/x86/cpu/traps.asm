extern int_no
extern err_code

%macro ISR_NOERR 1
global isr%1
isr%1:
    cli
    mov dword [err_code], 0
    mov dword [int_no], %1
    jmp isr_stub
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    pop dword [err_code]
    mov dword [int_no], %1
    jmp isr_stub
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    cli
    mov dword [err_code], 0
    mov dword [int_no], %2
    jmp irq_stub
%endmacro

%macro pushcontext 0
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi
    mov eax, ds
    push eax
%endmacro

%macro popcontext 0
    pop eax
    mov ds, ax
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax
%endmacro

ISR_NOERR 0
    ISR_NOERR 1
    ISR_NOERR 2
    ISR_NOERR 3
    ISR_NOERR 4
    ISR_NOERR 5
    ISR_NOERR 6
    ISR_NOERR 7
    ISR_ERR   8
    ISR_NOERR 9
    ISR_ERR   10
    ISR_ERR   11
    ISR_ERR   12
    ISR_ERR   13
    ISR_ERR   14
    ISR_NOERR 15
    ISR_NOERR 16
    ISR_NOERR 17
    ISR_NOERR 18
    ISR_NOERR 19
    ISR_NOERR 20
    ISR_NOERR 21
    ISR_NOERR 22
    ISR_NOERR 23
    ISR_NOERR 24
    ISR_NOERR 25
    ISR_NOERR 26
    ISR_NOERR 27
    ISR_NOERR 28
    ISR_NOERR 29
    ISR_NOERR 30
    ISR_NOERR 31
    ISR_NOERR 128
    IRQ   0,    32
    IRQ   1,    33
    IRQ   2,    34
    IRQ   3,    35
    IRQ   4,    36
    IRQ   5,    37
    IRQ   6,    38
    IRQ   7,    39
    IRQ   8,    40
    IRQ   9,    41
    IRQ  10,    42
    IRQ  11,    43
    IRQ  12,    44
    IRQ  13,    45
    IRQ  14,    46
IRQ  15,    47

extern isr_handler
extern irq_handler

isr_stub:
    pushcontext
    mov eax, esp
    push eax
    call isr_handler
    pop eax
    popcontext
    iret


irq_stub:
    pushcontext
    mov eax, esp
    push eax
    call irq_handler
    pop eax
    popcontext
    iret

global x86jmp_to
x86jmp_to:
    mov eax, [esp + 4]
    pushcontext
    mov dword[last_esp], esp
    mov esp, eax
    pop eax
    call eax
    mov esp, dword[last_esp]
    popcontext
    ret

last_esp dd 0

extern printf
_fmt db "contxt=%p", 0xa, 0xd, 0

extern cur_thread

global x86_spwanthread
x86_spwanthread:
    mov eax, [esp + 4]
    mov esp, [eax + 8]
    mov ebx, [eax + 4]
    xchg eax, ebx
    call eax
    ret


_eip dd 0
_esp dd 0