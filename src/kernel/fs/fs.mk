fat12_dir=$(fs_dir)\fat12
ramfs_dir=$(fs_dir)\ramfs
posix_dir=$(fs_dir)\posix

include $(posix_dir)\posix.mk
include $(fat12_dir)\fat12.mk
include $(ramfs_dir)\ramfs.mk

fs_objs=\
$(fs_dir)\itbl.o\
$(fs_dir)\vfs.o\
$(ramfs_objs)\
$(fat12_objs)\
$(posix_objs)