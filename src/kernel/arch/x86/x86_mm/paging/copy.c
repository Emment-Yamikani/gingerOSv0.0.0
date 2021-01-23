#include <stdio.h>
#include <string.h>
#include <mm\vm.h>
#include <mm\pm.h>
#include <mm\paging.h>
#include <sys\panic.h>

static char buff[PAGESIZE];

void memcpypp(uintptr_t _dst, uintptr_t _src, size_t sz)
{
    size_t nr = sz / PAGESIZE;
    uintptr_t prev_mount = mount_frame(0);
    while(nr--)
    {
        mount_frame(_src);
        memcpy((void *)buff, (void *)MOUNT_ADDR, PAGESIZE);
        mount_frame(_dst);
        memcpy((void *)MOUNT_ADDR, (void *)buff, PAGESIZE);
        _src += PAGESIZE;
        _dst += PAGESIZE;
    }
    sz %= PAGESIZE;
    if(sz){
        mount_frame(_src);
        memcpy((void *)buff, (void *)MOUNT_ADDR, sz);
        mount_frame(_dst);
        memcpy((void *)MOUNT_ADDR, (void *)buff, sz);
    }
    mount_frame(prev_mount);
}

void memcpypv(uintptr_t _dstv, uintptr_t _srcp, size_t sz)
{
    size_t nr = sz / PAGESIZE;
    uintptr_t prev_mount = mount_frame(_srcp);
    while(nr--){
        memcpy((void *)_dstv, (void *)MOUNT_ADDR, PAGESIZE);
        _dstv += PAGESIZE;
        _srcp += PAGESIZE;
        mount_frame(_srcp);
    }
    sz %= PAGESIZE;
    if(sz)
        memcpy((void *)_dstv, (void *)MOUNT_ADDR, sz);
    mount_frame(prev_mount);
}

void memcpyvp(uintptr_t _dstp, uintptr_t _srcv, size_t sz)
{
    size_t nr = sz / PAGESIZE;
    uintptr_t prev_mount = mount_frame(0);
    while(nr--){
        mount_frame(_dstp);
        memcpy((void *)MOUNT_ADDR, (void *)_srcv, PAGESIZE);
        _srcv += PAGESIZE;
        _dstp += PAGESIZE;
    }
    sz %= PAGESIZE;
    if(sz){
        mount_frame(_dstp);
        memcpy((void *)MOUNT_ADDR, (void *)_srcv, sz);
    }
    mount_frame(prev_mount);
}

void copy_fork_mapping(uintptr_t base, uintptr_t fork)
{
    switch_directory(fork);
    uintptr_t prev_mount = mount_frame(base);
    table_t *pd0 = (table_t *)MOUNT_ADDR;
    for(int i =0; i < USERTABLES; ++i)
    {
        if(pd0[i].structure.present){
            get_table(i, VM_URW);
            mount_frame((pd0[i].raw & ~PAGEMASK));
            page_t *t = (page_t *)MOUNT_ADDR;
            for(int j=0; j < 1024; ++j)
                if(t[j].structure.present){
                    PAGETBL(i)[j] = t[j];
                    if(t[j].structure.write){
                        t[j].structure.write =0;
                        PAGETBL(i)[j].structure.write =0;
                    }
                if (frames)
                    frames[(t[j].raw & ~PAGEMASK) / PAGESIZE].refs++;
                //!else postpone increment
            }
            mount_frame(base);
            pd0 = (table_t *)MOUNT_ADDR;
        }
    }
    mount_frame(prev_mount);
    switch_directory(base);
}