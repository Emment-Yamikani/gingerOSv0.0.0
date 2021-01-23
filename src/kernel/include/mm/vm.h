#ifndef MM_VM_H
# define MM_VM_H
#include <string.h>
#include <system.h>
#include <mm\32bit.h>
#include <sys\panic.h>


#define PAGESIZE 0x1000
#define TABLESIZE 0x400000
#define PAGEMASK 0xfff
#define TABLEMASK 0x3FFFFF

#define USERTABLES 768

#define VMENTRIES 0xC0000
#define KVMENTRIES 0x40000

#define LOWER_PAGE_BOUNDARY(ptr) ((ptr) & ~PAGEMASK)
#define UPPER_PAGE_BOUNDARY(ptr) (((ptr) + PAGEMASK) & ~PAGEMASK)

#define LOWER_TABLE_BOUNDARY(ptr) ((ptr) & ~TABLEMASK)
#define UPPER_TABLE_BOUNDARY(ptr) (((ptr) + TABLEMASK) & ~TABLEMASK)

#define VMA_BASE 0xC0000000


#define USER_STACK 0xC0000000
#define USER_STACK_SIZE 0x2000
#define USER_STACK_BASE (USER_STACK - USER_STACK_SIZE)

#define KERNEL_STACK 0xFFBFF000
#define KERNEL_STACK_SIZE 0x4000
#define KERNEL_STACK_BASE (KERNEL_STACK - KERNEL_STACK_SIZE)

#define PAGE_PRESENT    _BS(0)
#define PAGE_WRITABLE   _BS(1)
#define PAGE_USER       _BS(2)
#define PAGE_ACCESSED   _BS(5)
#define PAGE_DIRTY      _BS(6)


#define VM_KRW          (PAGE_PRESENT | PAGE_WRITABLE)
#define VM_URW          (PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER)

struct _vman
{
    void (*map_to_phy)(uintptr_t page, size_t sz, uintptr_t flags);
    void (*map_phy_to_vir)(uintptr_t frame, uintptr_t page, size_t sz, uintptr_t flags);
    page_t *(*get_mapping)(uintptr_t page);
    void (*unmap)(uintptr_t page, size_t sz);
    void *(*memcpypv)(uintptr_t _dstv, uintptr_t _srcp, size_t sz);
    void *(*memcpyvp)(uintptr_t _dstp, uintptr_t _srcv, size_t sz);
    void *(*memcpypp)(uintptr_t _dstp, uintptr_t _srcp, size_t sz);
    void (*switch_mapping)(uintptr_t pd);
    void (*copy_fork_mapping)(uintptr_t base, uintptr_t fork);
    void (*init_mem)(void *base, size_t sz);
    uintptr_t (*mount)(uintptr_t);
};

static struct _vm
{
    size_t maxblocks; //vRAM in blocks
    size_t usedblocks;
    size_t freeblocks;
} __packed vmem;

/*
*             BITMAP            
*track the physical adrees space
*/
#define VM_MAX_INTS 0x2000
static uintptr_t kvm_higher_bitmap[VM_MAX_INTS];

static inline void init_vmbitmap()
{
    memset(kvm_higher_bitmap, 0, sizeof(kvm_higher_bitmap));
    vmem.maxblocks  = (size_t)(VM_MAX_INTS * 32);
    vmem.freeblocks = (size_t)vmem.maxblocks;
    vmem.usedblocks = (size_t)NULL;
}

static inline int vm_ftest(int b)
{
    return (kvm_higher_bitmap[b / 32] & (1 << (b % 32)));
}

static inline int vm_fset(int b)
{
    if (vm_ftest(b))
        panic("vm_ftest: page(%d) already set", b);
    --vmem.freeblocks;
    ++vmem.usedblocks;
    return (kvm_higher_bitmap[b / 32] |= (1 << (b % 32)));
}

static inline int vm_ffree()
{
    int i, j;
    for (i = 0; i < VM_MAX_INTS; ++i)
        if (kvm_higher_bitmap[i] != 0xffffffff)
            for (j = 0; j < 32; ++j)
                if (!(kvm_higher_bitmap[i] & (1 << j)))
                    return (i * 32 + j);
    return -1;
}

static inline int vm_ffreefrom(int b)
{
    int i, j;
    for (i = (b / 32); i < VM_MAX_INTS; ++i)
        if (kvm_higher_bitmap[i] != 0xffffffff)
            for (j = (b % 32); j < 32; ++j)
                if (!(kvm_higher_bitmap[i] & (1 << j)))
                    return (i * 32 + j);
    return -1;
}

static inline int vm_ffreepgs(size_t pgs)
{
    size_t sz = pgs;
    int bit = vm_ffree();
    int ret = 0;
    ret = bit;
    if((ret != (int)(-1))){
        while (sz--)
        {
            if (!vm_ftest(bit))
                bit++;
            else
            {
                bit = vm_ffreefrom(bit);
                if(bit == -1)
                    return bit;
                ret = bit;
                sz = pgs;
                continue;
            }
        }
        return ret;
    }
    return -1;
}

static inline int vm_funset(int b)
{
    if (!vm_ftest(b))
        return 0;
    ++vmem.freeblocks;
    --vmem.usedblocks;
    return (kvm_higher_bitmap[b / 32] &= ~(1 << (b % 32)));
}


extern struct _vmman vmman;
extern void setup_x86_paging();
extern void tlb_flush();
#endif