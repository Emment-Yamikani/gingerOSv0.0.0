#ifndef SYS_THREAD_H
# define SYS_THREAD_H
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/process.h>

typedef enum
{
    tRUNNABLE, tISLEEP, tUSLEEP, tZOMBIE
}thread_state_t;


typedef struct task
{
    /*pointer to archtecture specific thread control block*/
    void *arch_thread;
    tid_t tid;
    thread_state_t state;
    process_t * owner;

    struct task *next;
    /*thread flags*/

    int spawnned:1;
    int user:1;
}__packed thread_t;


extern thread_t *cur_thread;

#endif