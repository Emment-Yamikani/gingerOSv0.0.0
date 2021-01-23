#include <mm\vm.h>
#include <mm\pm.h>
#include <sizes.h>
#include <stdio.h>
#include <string.h>
#include <mm\bitmap.h>
#include <mm\alloc_page.h>
#include <boot\boot.h>
#include <arch\x86\cpu\isrs.h>
#include <arch\x86\cpu\regs.h>

extern void _kernel_end();

void pmm_setup(size_t msz)
{
    init_bitmap();
    pmman.memsz = msz;
    if ((pmman.memsz / _1K) < 16)
        panic("(%dMiB)Memory is too low...\nAt least 16MiB is need for ginger to operate efficiently bailling out!", (pmman.memsz / _1K));
    pmman.freeblocks = pmman.maxblocks = pmman.memsz / 4;
    max_ints = pmman.maxblocks / 32;
    pmman.usedblocks = 0;
    size_t sz = (size_t)(_kernel_end - VMA_BASE) / PAGESIZE;
    if((size_t)(_kernel_end - VMA_BASE) % PAGESIZE)++sz;
    ++max_ints;
    allocframe_range((void *)0, sz);
    for (size_t i = 0; i < boot_info.mmap_count; ++i)
        if (boot_info.mmap[i].type == 1 && boot_info.mmap[i].base < _1M)
            freeframes((uintptr_t)boot_info.mmap[i].base, (size_t)boot_info.mmap[i].len);
    pm_fset(0);
    for(size_t i=0; i < boot_info.modules_count; ++i)
    {
        uintptr_t frame = boot_info.modules[i].addr - VMA_BASE;
        size_t sz = boot_info.modules[i].size;
        size_t pgs = sz / PAGESIZE;
        if (sz % PAGESIZE)
            pgs++;
        for (; pgs; --pgs, frame += PAGESIZE)
            pm_fset(frame / PAGESIZE);
    }
    table_t *pd = (table_t *)(_read_cr3() & ~PAGEMASK);
    pd[0].raw = 0;
    tlb_flush();
}

/*=================================PAGE FRAME HELPERS===============================*
**==================================================================================*/

/*alloc 1 pageframe*/
void *allocframe()
{
    int free = pm_ffree();
    if (free == -1)
        return NULL;
    pm_fset(free);
    return (void *)(free * PAGESIZE);
}

/*alloc (sz) page frames from base*/
void allocframe_range(void *base, size_t pgs)
{
    if (!pgs)
        panic("allocframe_range: can't alloc 0 pages\n");
    uint32_t bit = (uint32_t)base / PAGESIZE;
    while (pgs--)
        pm_fset(bit++);
}

/*alloc (sz) page frames*/
void *allocframes(size_t pgs)
{
    if (!pgs)
        panic("allocframes: can't alloc 0 frames");
    int keep = pm_ffree();
    try:
        for (size_t i = 0; i < pgs; ++i)
            if (!pm_ftest(keep + i))
                continue;
            else
            {
                keep = pm_ffreefrom(keep + i);
                if (keep == -1)
                    return 0;
                goto
                try
                    ;
            }
    for (size_t i = 0; i < pgs; ++i)
        pm_fset(keep + i);
    return (void *)(keep * PAGESIZE);
}

void freeframe(uintptr_t frame)
{
    pm_funset(frame / PAGESIZE);
}

void freeframes(uintptr_t base, size_t sz)
{
    uint32_t pgs = sz / PAGESIZE;
    if (sz % PAGESIZE)
        ++pgs;
    base /= PAGESIZE;
    while (base < pgs)
        pm_funset(base++);
}