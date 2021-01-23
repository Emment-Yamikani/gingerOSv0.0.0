#include <stdio.h>
#include <sys\thread.h>
#include <mm\kmalloc.h>
#include <arch\x86\cpu\context.h>
#include <arch\x86\cpu\x86thread.h>
#include <sys\sched.h>
#include <sys\cdefs.h>


process_t *ginger;

pid_t pID = 0;

void init_tasking()
{
    ginger = (process_t*)kmalloc(sizeof(*ginger));
    cur_thread = (thread_t *)kmalloc(sizeof(*cur_thread));
    if(ginger)
        memset((void *)ginger, 0, sizeof(*ginger));
    if(cur_thread)
        memset((void *)cur_thread, 0, sizeof(*cur_thread));
    ginger->pid = (pid_t)pID++;
    ginger->threads = 1;
}