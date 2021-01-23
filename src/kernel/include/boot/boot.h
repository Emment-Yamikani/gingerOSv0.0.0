#ifndef INC_BOOT_BOOT_H
# define INC_BOOT_BOOT_H 1
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sizes.h>
#include <sys\cdefs.h>
#include <system.h>
#include "multiboot.h"

#ifndef VMA_HIGH
#define VMA_HIGH(v) ((uintptr_t)0xc0000000 + v)
#endif

struct _region
{
    size_t size;
    uint64_t base;
    uint64_t len;
    uint32_t type;
} __packed;

typedef struct _module{
    char *cmdline; /*module command line*/
    uintptr_t addr; /*stating address of module*/
    size_t size; /*size of module*/
}module_t;


typedef struct _boot{
    size_t pm_size; /*physical memory size in kB*/
    char *cmdline; /*kernel command line*/
    module_t modules[8]; /*array of modules*/
    size_t modules_count; /*number of modules*/
    struct _region mmap[32]; /*memory map*/
    size_t mmap_count; /*count of memory maps*/
}boot_t;

/*boot information*/
extern boot_t boot_info;

static inline void get_mmap(multiboot_info_t *info)
{
    int idx = 0;
    if ((uint32_t)info->flags & SHL(1, 6))
    {
        multiboot_memory_map_t *mmap_buff_entry = (multiboot_memory_map_t *)info->mmap_addr;
        size_t endptr = (size_t)mmap_buff_entry + (size_t)info->mmap_length;
        uint32_t hold = (uint32_t)mmap_buff_entry;
        while ((size_t)mmap_buff_entry < endptr)
        {
            ++boot_info.mmap_count;
            boot_info.mmap[idx].base = mmap_buff_entry->addr;
            boot_info.mmap[idx].len = mmap_buff_entry->len;
            boot_info.mmap[idx++].type = mmap_buff_entry->type;
            boot_info.pm_size += mmap_buff_entry->len / _1K;
            mmap_buff_entry = (multiboot_memory_map_t *)(hold += (uint32_t)mmap_buff_entry->size + sizeof(mmap_buff_entry->size));
        }
    }
}

static inline void build_module_mapping(multiboot_info_t *info)
{
    multiboot_module_t *mods = (multiboot_module_t *)info->mods_addr;

    for (unsigned i = 0; i < info->mods_count; ++i)
    {
        boot_info.modules[i] = (module_t){
            .addr = (uintptr_t)VMA_HIGH(mods[i].mod_start),
            .size = mods[i].mod_end - mods[i].mod_start,
            .cmdline = (char *)VMA_HIGH(mods[i].cmdline)
        };
    }
}


static inline void get_multiboot_info(multiboot_info_t *info)
{
    memset((void *)&boot_info, 0, sizeof(boot_info));
    /*get commant line*/
    boot_info.cmdline = ((char *)VMA_HIGH((uintptr_t)info->cmdline));
    /*get memory map*/
    get_mmap(info);
    /*build module map*/
    build_module_mapping(info);
    boot_info.modules_count = (size_t)info->mods_count;
}

#endif