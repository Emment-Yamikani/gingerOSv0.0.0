#ifndef X86_CPU_CONTEXT_H
# define X86_CPU_CONTEXT_H

#include <stdint.h>
#include <sys\cdefs.h>


typedef struct x86_kstate
{
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
}__packed x86_kregs_t;

typedef struct x86_ustate
{
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __packed x86_uregs_t;

#endif