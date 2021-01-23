#ifndef MM_BITMAP_H
#define MM_BITMAP_H 1
#include <stdio.h>
#include <string.h>
#include <sizes.h>
#include <stdbool.h>
#include <sys\panic.h>
#include <stdint.h>
#include <mm\pm.h>
#include <mm\vm.h>

#define MAX_INTS 0x8000

/*
*             BITMAP            
*track the physical adrees space
*/
static uint32_t pbitmap[MAX_INTS];
static uint32_t max_ints=0;

static inline void init_bitmap()
{
    memset(pbitmap, 0, sizeof(pbitmap));
}

static inline uint32_t pm_ffree()
{
    uint32_t i, j;
    for (i = 0; i < max_ints; ++i)
        if (pbitmap[i] != 0xffffffff)
            for (j = 0; j < 32; ++j)
                if (!(pbitmap[i] & (1 << j)))
                    return (i * 32 + j);
    panic("ffree: no free frame was found");
    return -1;
}

static inline uint32_t pm_ffreefrom(uint32_t b)
{
    uint32_t i, j;
    for (i = (b/32); i < max_ints; ++i)
        if (pbitmap[i] != 0xffffffff)
            for (j = (b%32); j < 32; ++j)
                if (!(pbitmap[i] & (1 << j)))
                    return (i * 32 + j);
    panic("ffree: no free frame was found");
    return -1;
}

static inline uint32_t pm_ftest(uint32_t b)
{
    return (pbitmap[b / 32] & (1 << (b % 32)));
}

static inline uint32_t pm_fset(uint32_t b)
{
    if(pm_ftest(b))
        return b;
    --pmman.freeblocks;
    ++pmman.usedblocks;
    return (pbitmap[b / 32] |= (1 << (b % 32)));
}

static inline uint32_t pm_funset(uint32_t b)
{
    if(!pm_ftest(b))
        return 0;
    ++pmman.freeblocks;
    --pmman.usedblocks;
    return (pbitmap[b / 32] &= ~(1 << (b % 32)));
}

#endif //MM_BITMAP_H was defined here