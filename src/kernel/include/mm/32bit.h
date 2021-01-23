#ifndef _PAGING_BIT_H
# define _PAGING_BIT_H
#include <stdint.h>
#include <stddef.h>
#include <sys\cdefs.h>


typedef union
{
    struct
    {
        size_t present : 1;
        size_t write : 1;
        size_t user : 1;
        size_t page_level_write_through : 1;
        size_t page_level_cache_disable : 1;
        size_t accessed : 1;
        size_t __ignored_0 : 1;
        size_t page_size : 1;
        size_t __ignored_1 : 4;
        uintptr_t phys_addr : 20;
    }__packed structure;
    uint32_t raw;
} __packed table_t;

typedef union
{
    struct
    {
        size_t present : 1;
        size_t write : 1;
        size_t user : 1;
        size_t page_level_write_through : 1;
        size_t page_level_cache_disable : 1;
        size_t accessed : 1;
        size_t dirty : 1;
        size_t memory_type : 1;
        size_t global : 1;
        size_t __ignored : 3;
        uintptr_t phys_addr : 20;
    } __packed structure;
    uint32_t raw;
} __packed page_t;

typedef union
{
    struct
    {
        size_t offset : 12;
        size_t table : 10;
        size_t directory : 10;
    } __packed structure;
    uint32_t raw;
} __packed __virtaddr_t;

#define MOUNT_ADDR ((uintptr_t)0xffbff000)
#define PAGEDIR    ((table_t *)0xfffff000)
#define PAGETBL(i) ((page_t *)(0xffc00000 + (0x1000*i)))

#endif