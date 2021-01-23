#ifndef X86THREAD_H
# define X86THREAD_H
#include "context.h"
#include <sys\cdefs.h>


typedef struct
{
    uintptr_t kstack;
    uintptr_t eip;
    uintptr_t esp;
    uintptr_t ebp;
    uintptr_t eflags;
    uintptr_t eax; // return value when from syscall

    /*can be user or kernel thread state*/
    void *regs;

}__packed x86_thread_t;

#endif