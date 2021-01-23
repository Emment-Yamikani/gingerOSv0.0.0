blk_dir=$(dev_dir)\blk
chr_dir=$(dev_dir)\chr

include $(blk_dir)\blk.mk
include $(chr_dir)\chr.mk

dev_objs=\
$(dev_dir)\kdev.o\
$(blk_objs)\
$(chr_objs)