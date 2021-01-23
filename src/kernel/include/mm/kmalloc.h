#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#define KMALLOC 1
extern void *kmalloc(size_t nbytes);
extern void *kcalloc(size_t nelem, size_t szelem);
extern void kfree(void *ptr);

#endif