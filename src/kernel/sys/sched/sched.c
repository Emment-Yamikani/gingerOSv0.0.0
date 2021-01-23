#include <stdio.h>
#include <sys\thread.h>
#include <arch\x86\cpu\regs.h>
#include <arch\x86\cpu\x86thread.h>

thread_t *cur_thread;

extern void x86jmp_to_user(int eip, int esp);
extern void x86_spwanthread(x86_thread_t*);

void switch_to(thread_t *t)
{
    x86_thread_t *tarch = t->arch_thread;
    x86_spwanthread(tarch);
}

static void kernel_idle()
{
    printf("no task to schedule\n");
    while(1){
        asm volatile("cli");
        asm volatile("hlt");
    }
}

void sched()
{
    cur_thread = cur_thread->next;
    if(cur_thread)
        switch_to(cur_thread);
    else
        kernel_idle();
}