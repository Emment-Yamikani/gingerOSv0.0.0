#ifndef SCHED_H
# define SCHED_H
#include <sys/thread.h>

extern void sched();
extern void switch_to(thread_t *);

#endif