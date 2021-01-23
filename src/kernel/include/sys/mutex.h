#ifndef KERNEL_MUTEX_H
# define KERNEL_MUTEX_H

typedef struct spinlock
{
    int locked;//is the lock held
    char *name;//lock name
}spinlock_t, mutex_t;

void acquire(struct spinlock *);
void release(struct spinlock *);

#endif