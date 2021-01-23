#ifndef INCLUDE_MMU_PAGEALLOC_H
#define INCLUDE_MMU_PAGEALLOC_H 1
#include <stddef.h>
#include <stdint.h>

extern void *alloc_page();
extern void *alloc_mapped_page();
extern void *alloc_contiguous(size_t pgs);
extern void *alloc_mapped_contiguous(size_t pgs);
extern void alloc_page_range(uintptr_t base, size_t pgs);
extern void free_pages(uintptr_t base, size_t pgs);

#endif //INCLUDE_MMU_PAGEALLOC_H was defined here