#include <stdio.h>
#include <stddef.h>
#include <mm\vm.h>
#include <mm\paging.h>

void *alloc_page()
{
    int bit = vm_ffree();
    if (bit != -1)
    {
        vm_fset(bit);
        return (void *)((bit * PAGESIZE) + VMA_BASE);
    }
    return NULL;
}

void *alloc_mapped_page()
{
    void *page = alloc_page();
    page_map((uintptr_t)page, VM_KRW);
    return page;
}

void *alloc_contiguous(size_t pgs)
{
    int bit = vm_ffreepgs(pgs);
    int ret = bit;
    if ((bit != (int)(-1)))
    {
        while (pgs--)
            vm_fset(bit++);
        return (void *)((ret * PAGESIZE) + VMA_BASE);
    }
    return NULL;
}

void *alloc_mapped_contiguous(size_t pgs)
{
    void *page = alloc_contiguous(pgs);
    if (page)
    {
        map_virtual_to_physical((uintptr_t)page, pgs * PAGESIZE, VM_KRW);
        return page;
    }
    return NULL;
}

void alloc_page_range(uintptr_t base, size_t pgs)
{
    int bit = ((base - VMA_BASE) / PAGESIZE);
    size_t pdi = 1024, pti = 1024;
    while (pgs--)
    {
        pdi = ((__virtaddr_t)base).structure.directory;
        pti = ((__virtaddr_t)base).structure.table;
        vm_fset(bit++);
        page_alloc(pdi, pti, VM_KRW);
        base += PAGESIZE;
    }
}

void free_pages(uintptr_t base, size_t pgs)
{
    int bit = (base - VMA_BASE) / PAGESIZE;
    size_t pdi = 1024, pti = 1024;
    while (pgs--)
    {
        pdi = ((__virtaddr_t)base).structure.directory;
        pti = ((__virtaddr_t)base).structure.table;
        vm_funset(bit++);
        page_delloc(pdi, pti);
        base += PAGESIZE;
    }
}