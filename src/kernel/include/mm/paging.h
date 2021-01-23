#ifndef MM_PAGING_H
# define MM_PAGING_H 1
#include <stdint.h>

/*
*MISC HELPERS */

extern void tlb_flush();
extern uintptr_t mount_frame(uintptr_t frame);
extern void switch_directory(uintptr_t next_pd);
extern void tlb_invalidate_page(uintptr_t virt);

/*
*PHYSICAL FRAME HELPERS  */

extern uintptr_t get_frame();
extern uintptr_t get_frame_no_clr();
extern void release_frame(uintptr_t frame);

/*
*TABLE HELPERS  */

extern void get_table(uint32_t pdi, uint32_t flags);
extern void release_table(uint32_t pdi);

/*
*PAGE HELPERS*/

extern void page_alloc(uint32_t pdi, uint32_t pti, uint32_t flags);
extern void page_delloc(uint32_t pdi, uint32_t pti);
extern void map_physical(uintptr_t frame, uint32_t pdi, uint32_t pti, uint32_t flags);
extern void page_map_to_physical(uintptr_t page, uint32_t flags);
extern void page_unmap(uint32_t page);
extern page_t *get_page_mapping(uintptr_t page);
extern void page_map(uintptr_t page, uint32_t flags);
extern void map_virtual_to_physical(uintptr_t page, size_t sz, uint32_t flags);
extern void map_physical_to_virtual(uintptr_t frame, uintptr_t page, size_t sz, uint32_t flags);
extern void unmap_from_physical(uintptr_t page, size_t sz);

/*
*COPY HELPERS*/
extern void memcpypp(uintptr_t _dst, uintptr_t _src, size_t sz);
extern void memcpypv(uintptr_t _dstv, uintptr_t _srcp, size_t sz);
extern void memcpyvp(uintptr_t _dstp, uintptr_t _srcv, size_t sz);
extern void copy_fork_mapping(uintptr_t base, uintptr_t fork);
#endif