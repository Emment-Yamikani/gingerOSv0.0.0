#ifndef MM_PM_H
# define MM_PM_H
#include <mm\vm.h>
#include <mm\32bit.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys\panic.h>
#include <sys\mutex.h>
#include <sys\cdefs.h>
#include <boot\multiboot.h>


static struct
{
    uintptr_t refs;
} *frames = NULL; /*frame refs, precaution as not to delloc a page in use*/

static struct _postpone
{
    struct _postpone *next;
    uintptr_t frame;
}postpone_list;

struct _pm
{
    size_t memsz;     //RAM in kB
    size_t maxblocks; //RAM in blocks
    size_t usedblocks;
    size_t freeblocks;
} __packed pmman;

//static mutex_t pm_lock;
static int scratch_idx =0;
static bool postpone_flag;
static char scratch_space[0x4000] = {0};

static inline void postpone_ref_inc(uintptr_t frame)
{
    #ifndef KMALLOC
    struct _postpone *next = (struct _postpone *)&scratch_space[scratch_idx];
    scratch_idx += sizeof(struct _postpone);
    #else
    struct _postpone *next = kmalloc(sizeof(struct _postpone));
    #endif
    next->frame = frame;
    next->next = (struct _postpone*)NULL;
    struct _postpone *tmp = &postpone_list;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = next;
    if(!postpone_flag)
        postpone_flag = true;
}

static inline void inc_postponed()
{
    /**
     * acquire lock here*/
    if(!frames)
        panic("inc_postpone: frames still null\n");
    struct _postpone * tmp = &postpone_list;
    while(tmp)
    {
        frames[tmp->frame / PAGESIZE].refs++;
        tmp = tmp->next;
    }
    postpone_list.next = (struct _postpone*)NULL;
    postpone_list.frame = (uintptr_t)NULL;
    postpone_flag = false;
    /**
     * release lock  here*/
}

extern void pmm_setup(size_t);
extern void *allocframe();
extern void allocframe_range(void *base, size_t sz);
extern void *allocframes(size_t sz);
extern void freeframe(uintptr_t frame);
extern void freeframes(uintptr_t base, size_t sz);

#endif