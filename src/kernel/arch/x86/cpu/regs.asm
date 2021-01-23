bits 32

global gdt_flush
global idt_flush
global tss_flush
global _read_cr0
global _read_cr2
global _read_cr3
global _read_cr4
global _read_esp
global _read_ebp
global _read_eip

global _write_cr0
global _write_cr3
global _write_cr4
global _write_esp
global _write_ebp



gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

tss_flush:
    mov ax, 0x2B
    ltr ax
    ret

_read_cr0:
    mov eax, cr0
    ret
_read_cr2:
    mov eax, cr2
    ret
_read_cr3:
    mov eax, cr3
    ret
_read_cr4:
    mov eax, cr4
    ret
_read_esp:
    mov eax, esp
    ret
_read_ebp:
    mov eax, ebp
    ret
_read_eip:
    mov eax, dword[esp +4]
    ret


_write_cr0:
    mov eax, dword [esp + 4]
    mov cr0, eax
    ret

_write_cr3:
    mov eax, dword [esp + 4]
    mov cr3, eax
    ret
_write_cr4:
    mov eax, dword [esp + 4]
    mov cr4, eax
    ret
_write_esp:
    mov eax, dword [esp + 4]
    mov esp, eax
    ret
_write_ebp:
    mov eax, dword [esp + 4]
    mov ebp, eax
    ret