#ifndef SYS_PROCESS_H
# define SYS_PROCESS_H
#include <mm\vm.h>
#include <stdint.h>
#include <fs\vfs.h>
#include <sys\cdefs.h>


#define MAX_FDS 32


typedef struct
{
    int pid;
    uintptr_t next_stackptr;
    uintptr_t heap_start;
    uintptr_t heap;
    size_t threads;

    file_t *fd;
}__packed process_t, proc_t;

extern pid_t pID;
extern proc_t *cur_proc;


extern int proc_get_fd(proc_t *);
extern int proc_release_fd(proc_t *, int);

#endif