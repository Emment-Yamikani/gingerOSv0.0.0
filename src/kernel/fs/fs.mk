ramfs_dir=$(fs_dir)\ramfs
posix_dir=$(fs_dir)\posix

include $(posix_dir)\posix.mk
include $(ramfs_dir)\ramfs.mk

fs_objs=\
$(fs_dir)\vfs.o\
$(ramfs_objs)\
$(posix_objs)

#$(posix_objs)