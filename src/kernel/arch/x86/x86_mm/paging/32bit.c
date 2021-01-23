#include <stdio.h>
#include <sizes.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <mm\vm.h>
#include <mm\pm.h>
#include <mm\32bit.h>
#include <mm\paging.h>
#include <mm\alloc_page.h>
#include <sys\panic.h>
#include <sys\cdefs.h>
#include <arch\x86\cpu\isrs.h>
#include <arch\x86\cpu\regs.h>
#include <boot\boot.h>

extern void tbl1022();
static uintptr_t cur_pd =(uintptr_t)NULL;
static page_t *last_table = (page_t *)tbl1022;
static uintptr_t bootstrap_pd;

void tlb_invalidate_page(uintptr_t virt)
{
    asm volatile ("invlpg (%%eax)" ::"a"(virt));
}

void tlb_flush()
{
    _write_cr3(_read_cr3());
}

void init_area(void *base, size_t sz)
{
    memset(base, 0, sz);
}

uintptr_t mount_frame(uintptr_t frame)
{
    uintptr_t prev_mount = (uintptr_t)last_table[1023].raw & ~PAGEMASK;
    last_table[1023].raw = frame & ~PAGEMASK;
    last_table[1023].structure.present =1;
    last_table[1023].structure.write =1;
    tlb_invalidate_page((uintptr_t)MOUNT_ADDR);
    tlb_invalidate_page((uintptr_t)MOUNT_ADDR);
    return prev_mount;
}

uintptr_t get_frame()
{
    uintptr_t frame = (uintptr_t)allocframe();
    uintptr_t prev_mount = mount_frame(frame);
    init_area((void *)MOUNT_ADDR, PAGESIZE);
    mount_frame(prev_mount);
    return frame;
}

uintptr_t get_frame_no_clr()
{
    return (uintptr_t)allocframe();
}

void release_frame(uintptr_t frame)
{
    freeframe(frame);
}

void switch_directory(uintptr_t next_pd)
{
    if(next_pd == cur_pd)return;
    if(cur_pd)
        memcpypp(cur_pd, bootstrap_pd, (size_t)(USERTABLES * 4));
    memcpypp(bootstrap_pd, next_pd, (size_t)(USERTABLES * 4));
    cur_pd = next_pd;
    tlb_flush();
}

static void pagefault_handler(void *arch)
{
    uintptr_t pgf = _read_cr2();

    panic("stack=(0x%p) pagefault@(%p) errcode[%b]\n", arch, pgf, err_code);
}

void setup_x86_paging()
{
    bootstrap_pd = (uintptr_t)(_read_cr3() & ~PAGEMASK);
    init_vmbitmap();
    install_isr_handler(0x0e, pagefault_handler);
    /*identity map kernel modules*/
    for(size_t i=0; i < boot_info.modules_count; ++i)
    {
        uintptr_t addr = boot_info.modules[i].addr;
        size_t sz = boot_info.modules[i].size;
        map_physical_to_virtual(addr - VMA_BASE, addr, sz, VM_KRW);
    }
    frames = (typeof(frames))alloc_mapped_contiguous((size_t)(pmman.memsz / _4K));
    /*todo: mark identity pages*/
}