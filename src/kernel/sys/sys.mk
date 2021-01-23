sched_dir=$(sys_dir)\sched
usr_dir=$(sys_dir)\usr

include $(usr_dir)\usr.mk
include $(sched_dir)\sched.mk

sys_objs=\
$(sys_dir)\panic.o\
$(sys_dir)\proc.o\
$(sys_dir)\unistd.o\
$(sys_dir)\syscall.o\
$(sched_objs)\
$(usr_objs)