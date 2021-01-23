bits 32
align 4
mboot:
;Declare constants for the multiboot header.
MULTIBOOT_ALIGN equ 1<<0             ;align loaded modules on page boundaries
MEMINFO         equ 1<<1             ;provide memory map
MAGIC           equ 0x1BADB002       ;'magic number' lets bootloader find the header
FLAGS           equ MULTIBOOT_ALIGN | MEMINFO  ;this is the Multiboot 'flag' field
CHECKSUM        equ -(MAGIC + FLAGS) ;checksum of above, to prove we are multiboot

;Declare a header as in the Multiboot Standard.
section .multiboot
align 4
    dd  MAGIC
    dd  FLAGS
    dd  CHECKSUM
    dd  mboot

section .text
align 4
extern _init
extern early_init
extern _kernel_end

%define vma_base 0xC0000000
%define _kstack  0xffbff000
%define pagesz 0x1000

global _start
_start:
    mov esi, eax ; preserve multiboot magic
    xor eax, eax
.setup_paging:
    lea edi, [sys_pd - vma_base]
    lea eax, [tbl0 - vma_base]
    or eax, 3
    mov dword[edi], eax         ;0
    mov dword[edi + 0xc00], eax ;768
    lea eax, [tbl1022 - vma_base]
    or eax, 3
    mov dword[edi + 0xff8], eax ;1022
    lea eax, [sys_pd - vma_base]
    or eax, 3
    mov dword[edi + 0xffc], eax  ;1023

    lea edi, [tbl0 -vma_base]
    xor edx, edx
    mov eax, (_kernel_end - vma_base)
    mov ecx, pagesz
    div ecx
    cmp edx, 0
    je .ready_to_map
    inc eax

.ready_to_map:
    mov ecx, eax
    xor eax, eax
    or eax, 3

.identity_map:
    mov dword[edi], eax
    add edi, 4
    add eax, pagesz
    loop .identity_map

    lea edi, [tbl1022 -vma_base]
    add edi, 0xff8
    mov ecx, 4
    lea eax, [(stack_top - pagesz) - vma_base]
    or eax, 3
.map_stack:
    mov dword[edi], eax
    sub edi, 4
    sub eax, pagesz
    loop .map_stack
    lea edi, [tbl1022 -vma_base]
    add edi, 0xffc
    lea eax, [_mount_frame - vma_base]
    or eax, 3
    mov dword[edi], eax
.enable_paging:
    lea eax, [sys_pd - vma_base]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    lea eax, [.higher_half]
    jmp eax
.higher_half:
    mov eax, cr3
    mov cr3, eax
    lea esp, [_kstack]
    mov ebp, esp
    call _init
    push ebx
    call early_init
._hlt:
    cli
    hlt
    jmp ._hlt

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

global sys_pd
global _mount_page
global tbl1022


align 0x1000
_mount_frame:
resb 0x1000
align 0x1000
sys_pd:
resb 0x1000
align 0x1000
tbl0:
resb 0x1000
align 0x1000
tbl1022:
resb 0x1000