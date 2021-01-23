ar=i686-elf-ar
as=i686-elf-as
cc=i686-elf-gcc

cflags?=-O2 -g -nostdlib -nostdinc -lgcc
cppflags?=
ldflags?=
libs?=


MAKEFLAGS += --no-print-directory

cflags:=$(cflags) -ffreestanding -Wall -Wextra

#-Werror

cppflags:=$(cppflags) -D__is_kernel -Iinclude
ldflags:=$(ldflags)

libk_dir=..\libk

libs:=\
$(libk_dir)\libk.a


kernel_dir=

arch_dir=arch
core_dir=core
dev_dir=dev
fs_dir=fs
mm_dir=mm
sys_dir=sys
src_dir=..
iso_dir=$(src_dir)\..\iso


libk_dir=..\libk

include $(arch_dir)\arch.mk
include $(core_dir)\core.mk
include $(dev_dir)\dev.mk
include $(fs_dir)\fs.mk
include $(mm_dir)\mm.mk
include $(sys_dir)\sys.mk

cflags:=$(cflags) $(KERNEL_ARCH_cflags)
cppflags:=$(cppflags) $(KERNEL_ARCH_cppflags)
ldflags:=$(ldflags) $(KERNEL_ARCH_ldflags)
shell=cmd


crti_obj=$(x86_dir)\crti.o
crtbegin_obj:=$(shell $(cc) $(cflags) -print-file-name=crtbegin.o)
crtend_obj:=$(shell $(cc) $(cflags) -print-file-name=crtend.o)
crtn_obj=$(x86_dir)\crtn.o



objs=\
$(arch_objs)\
$(core_objs)\
$(dev_objs)\
$(mm_objs)\
$(fs_objs)\
$(sys_objs)

internal_objs:=$(crti_obj) $(objs) $(crtn_obj)

obj_link_list:=\
$(crti_obj)\
$(crtbegin_obj)\
$(objs)\
$(libs)\
$(crtend_obj)\
$(crtn_obj)

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .asm

all: kernel.elf

all_clean: all clean


kernel.elf: $(obj_link_list) $(x86_dir)\linker.ld $(libk_dir)\libk.a
	$(cc) -T $(x86_dir)\linker.ld -o $(iso_dir)\boot\$@ $(cflags)  $(obj_link_list)

.c.o:
	$(cc) -MD -c $< -o $@ -std=gnu11 $(cflags) $(cppflags)

.S.o:
	$(cc) -MD -c $< -o $@ $(cflags) $(cppflags)

.asm.o:
	nasm $< -f elf32 -o $@

clean:
	del $(internal_objs) $(internal_objs:.o=.d)