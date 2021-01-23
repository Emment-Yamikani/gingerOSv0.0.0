boot_dir=$(x86_dir)\boot
cpu_dir=$(x86_dir)\cpu
x86_mm_dir=$(x86_dir)\x86_mm
chipset_dir=$(x86_dir)\chipset

include $(chipset_dir)\chipset.mk
include $(x86_mm_dir)\x86_mm.mk
include $(cpu_dir)\cpu.mk
include $(boot_dir)\boot.mk

x86_objs=\
$(x86_dir)\x86io.o\
$(boot_objs)\
$(cpu_objs)\
$(x86_mm_objs)\
$(chipset_objs)