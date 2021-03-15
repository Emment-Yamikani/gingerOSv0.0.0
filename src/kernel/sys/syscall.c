#include <stdio.h>
#include <sys/errno.h>
#include <fs/fs.h>
#include <sys/panic.h>
#include <sys/thread.h>
#include <arch/x86/cpu/isrs.h>
#include <arch/x86/cpu/regs.h>
#include <arch/x86/cpu/context.h>

#define max_syscalls 0

void x86_syscall_return(thread_t *t __unused, int ret_val __unused)
{
    
}


void sys_open(const char *path __unused, mode_t mode __unused)
{
    
}

void sys_read(int fd __unused, void *buf __unused, size_t size __unused)
{

}

void sys_write(int fd __unused, void *buf __unused, size_t size __unused)
{

}

void sys_lseek(int fd __unused, off_t offset __unused, int whence __unused)
{

}

void sys_close(int fd __unused)
{

}

void x86_dump_context(void *arch)
{
    uint32_t ds =0;
    uint32_t edi =0;
    uint32_t esi =0;
    uint32_t ebp =0;
    uint32_t ebx =0;
    uint32_t edx =0;
    uint32_t ecx =0;
    uint32_t eax =0;
    uint32_t eip =0;
    uint32_t cs =0;
    uint32_t eflags =0;
    uint32_t esp = _read_esp();
    uint32_t ss =0x10;
    if(cur_thread){
        if (cur_thread->user){
            ds = ((x86_uregs_t *)arch)->ds;
            edi = ((x86_uregs_t *)arch)->edi;
            esi = ((x86_uregs_t *)arch)->esi;
            ebp = ((x86_uregs_t *)arch)->ebp;
            ebx = ((x86_uregs_t *)arch)->ebx;
            edx = ((x86_uregs_t *)arch)->edx;
            ecx = ((x86_uregs_t *)arch)->ecx;
            eax = ((x86_uregs_t *)arch)->eax;
            eip = ((x86_uregs_t *)arch)->eip;
            cs = ((x86_uregs_t *)arch)->cs;
            eflags = ((x86_uregs_t *)arch)->eflags;
            esp = ((x86_uregs_t *)arch)->esp;
            ss = ((x86_uregs_t *)arch)->ss;
        }
        else{
            ds = ((x86_kregs_t *)arch)->ds;
            edi = ((x86_kregs_t *)arch)->edi;
            esi = ((x86_kregs_t *)arch)->esi;
            ebp = ((x86_kregs_t *)arch)->ebp;
            ebx = ((x86_kregs_t *)arch)->ebx;
            edx = ((x86_kregs_t *)arch)->edx;
            ecx = ((x86_kregs_t *)arch)->ecx;
            eax = ((x86_kregs_t *)arch)->eax;
            eip = ((x86_kregs_t *)arch)->eip;
            cs = ((x86_kregs_t *)arch)->cs;
            eflags = ((x86_kregs_t *)arch)->eflags;
        }
    }
    printf("\n+++++++++++thread context++++++++++\n");
    printf("_ds=%p edi=%p esi=%p ebp=%p\n", ds, edi, esi, ebp);
    printf("ebx=%p ebx=%p ecx=%p eax=%p\n", ebx, edx, ecx, eax);
    printf("+++++++++++++cpu state+++++++++++++\n");
    printf("eip=%p cs=%p eflags=%p esp=%p ss=%p\n", eip, cs, eflags, esp, ss);
}


void syscall_stub(void *arch)
{
    arch= arch;
    if(!max_syscalls)
        panic("not ready for syscalls yet");
}