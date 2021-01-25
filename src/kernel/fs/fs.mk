ramfs_dir=$(fs_dir)\ramfs_dir

include $(ramfs_dir)\ramfs.mk

fs_objs=\
$(fs_dir)\vfs.o\
$(ramfs_objs)\

#$(posix_objs)