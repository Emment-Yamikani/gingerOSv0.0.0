#include <stdint.h>
#include <mm\vm.h>
#include <mm\pm.h>
#include <mm\paging.h>
#include <sys\panic.h>


void get_table(uint32_t pdi, uint32_t flags)
{
    if (pdi > 1023)
        panic("get_table: (%d)pdi > 1023", pdi);
    uintptr_t frame;
    if(!PAGEDIR[pdi].structure.present){
        PAGEDIR[pdi].raw = frame = get_frame();
        PAGEDIR[pdi].raw |= flags & PAGEMASK;
        if (frames)
            frames[frame / PAGESIZE].refs++;
        //!else postpone increment
    }
    tlb_flush();
}

void release_table(uint32_t pdi)
{
    if (pdi > 1023)
        panic("release_table: (%d)pdi > 1023", pdi);
    if(PAGEDIR[pdi].structure.present){
        uintptr_t frame = PAGEDIR[pdi].raw & ~PAGEMASK;
        PAGEDIR[pdi].raw = 0;
        if (frames){
            if (frames[frame / PAGESIZE].refs == 1)
                release_frame(frame);
            frames[frame / PAGESIZE].refs--;
        }
    }
    tlb_flush();
}