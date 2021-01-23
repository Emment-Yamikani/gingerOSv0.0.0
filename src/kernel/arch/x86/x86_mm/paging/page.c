#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <mm\vm.h>
#include <mm\pm.h>
#include <mm\32bit.h>
#include <mm\paging.h>
#include <sys\panic.h>

void map_physical(uintptr_t frame, uint32_t pdi, uint32_t pti, uint32_t flags)
{
    if (frame & PAGEMASK)
        panic("map_physical: frame=%p not 4k aligned\n", frame);
    if (pdi > 1023 || pti > 1023)
        panic("map_physical: pdi=%d pti=%d\n", pdi, pti);
    if (!PAGEDIR[pdi].structure.present)
        get_table(pdi, flags);
    if (!PAGETBL(pdi)[pti].structure.present)
    {
        PAGETBL(pdi)[pti].raw = frame;
        PAGETBL(pdi)[pti].raw |= flags & PAGEMASK;
        if (frames)
            frames[frame / PAGESIZE].refs++;
        //!else postpone increment
    }
    __virtaddr_t page;
    page.structure.directory =pdi;
    page.structure.table = pti;
    tlb_invalidate_page(page.raw);
}

void page_alloc(uint32_t pdi, uint32_t pti, uint32_t flags)
{
    uintptr_t frame = get_frame_no_clr();
    map_physical(frame, pdi, pti, flags);
    __virtaddr_t page;
    page.structure.directory =pdi;
    page.structure.table = pti;
    tlb_invalidate_page(page.raw);
}

void page_delloc(uint32_t pdi, uint32_t pti)
{
    if (pdi > 1023 || pti > 1023)
        panic("page_delloc: pdi=%d pti=%d\n", pdi, pti);
    if(PAGEDIR[pdi].structure.present)
        if(PAGETBL(pdi)[pti].structure.present){
            uintptr_t frame = PAGETBL(pdi)[pti].raw & ~PAGEMASK;
            PAGETBL(pdi)[pti].raw = 0;
            if(frames){
                if (frames[frame / PAGESIZE].refs == 1)
                    release_frame(frame);
                frames[frame / PAGESIZE].refs--;
            }
        }
        __virtaddr_t page;
        page.structure.directory =pdi;
        page.structure.table = pti;
        tlb_invalidate_page(page.raw);
}

void page_map_to_physical(uintptr_t page, uint32_t flags)
{
    uint32_t pdi = ((__virtaddr_t)page).structure.directory;
    uint32_t pti = ((__virtaddr_t)page).structure.table;
    page_alloc(pdi, pti, flags);
    tlb_invalidate_page(page);
}

void page_map(uintptr_t page, uint32_t flags)
{
    page_map_to_physical(page, flags);
}

void page_unmap(uint32_t page)
{
    uint32_t pdi = ((__virtaddr_t)page).structure.directory;
    uint32_t pti = ((__virtaddr_t)page).structure.table;
    page_delloc(pdi, pti);
    tlb_invalidate_page(page);
}

page_t *get_page_mapping(uintptr_t page)
{
    size_t pdi = ((__virtaddr_t)page).structure.directory;
    size_t pti = ((__virtaddr_t)page).structure.table;
    if(PAGEDIR[pdi].structure.present)
        if(PAGETBL(pdi)[pti].structure.present)
            return &PAGETBL(pdi)[pti];
        else return (page_t *)NULL;
    else return (page_t *)NULL;
}

void map_physical_to_virtual(uintptr_t frame, uintptr_t page, size_t sz, uint32_t flags)
{
    size_t nr = sz / PAGESIZE;
    if(sz % PAGESIZE)++nr;
    while(nr--){
        uint32_t pdi = ((__virtaddr_t)page).structure.directory;
        uint32_t pti = ((__virtaddr_t)page).structure.table;
        map_physical(frame, pdi, pti, flags);
        tlb_invalidate_page(page);
        frame += PAGESIZE;
        page += PAGESIZE;
    }
}

void map_virtual_to_physical(uintptr_t page, size_t sz, uint32_t flags)
{
    size_t nr = sz / PAGESIZE;
    if(sz % PAGESIZE)++nr;
    while(nr--){
        page_map(page, flags);
        page += PAGESIZE;
    }
}

void unmap_from_physical(uintptr_t page, size_t sz)
{
    size_t nr = sz / PAGESIZE;
    if (sz % PAGESIZE)++nr;
    uintptr_t ptr = page;
    while(nr--){
        page_unmap(page);
        tlb_invalidate_page(page);
        page += PAGESIZE;
    }
    page = ptr;
    ptr = UPPER_TABLE_BOUNDARY(page);
    uintptr_t end = LOWER_TABLE_BOUNDARY(page + sz);
    nr = (end - ptr) / TABLESIZE;
    ptr /= TABLESIZE;
    while(nr--)
        release_table(ptr++);
}