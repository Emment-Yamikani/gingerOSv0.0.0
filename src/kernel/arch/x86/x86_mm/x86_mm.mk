paging_dir=$(x86_mm_dir)\paging

include $(paging_dir)\paging.mk

x86_mm_objs=\
$(paging_objs)