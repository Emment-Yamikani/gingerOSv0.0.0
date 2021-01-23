ar=i686-elf-ar
as=i686-elf-as
cc=i686-elf-gcc

cflags?=-O2 -g
cppflags?=
ldflags?=
libs?=


cflags:=$(cflags) -ffreestanding -Wall -Wextra
cppflags:=$(cppflags) -D__is_libc -Iinclude
libk_clags:=$(cflags)
libk_cppflags:=$(cppflags) -D__is_libk


arch_dir=arch
ctype_dir=ctype
string_dir=string
stdio_dir=stdio
stdlib_dir=stdlib

include $(arch_dir)\arch.mk
include $(ctype_dir)\ctype.mk
include $(stdio_dir)\stdio.mk
include $(stdlib_dir)\stdlib.mk
include $(string_dir)\string.mk



cflags:=$(cflags)
cppflags:=$(cppflags)
libk_clags:=$(libk_clags)
libk_cppflags:=$(libk_cppflags)



freeobjs=\
$(ctype_objs)\
$(stdio_objs)\
$(stdlib_objs)\
$(string_objs)



objs=\
$(freeobjs)\

libk_objs=$(freeobjs:.o=.libk.o)
libk_ds=$(freeobjs:.o=.libk.d)

#BINARIES=libc.a libk.a # Not ready for libc yet.
BINARIES=libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S

all: rema $(BINARIES)
	del $(libk_objs) $(libk_ds)

libk.a: $(libk_objs)
	$(ar) rcs $@ $(libk_objs)

.c.o:
	$(cc) -MD -c $< -o $@ -std=gnu11 $(cflags) $(cppflags)

.c.S:
	$(cc) -MD -c $< -o $@ $(cflags) $(cppflags)

.c.libk.o:
	$(cc) -MD -c $< -o $@ -std=gnu11 $(libk_clags) $(libk_cppflags)

.S.libk.o:
	$(cc) -MD -c $< -o $@ $(libk_clags) $(libk_cppflags)

clean:
	del $(BINARIES) *.a
	del $(libk_objs) $(libk_ds)

rema:
	del $(BINARIES) *.a

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include\. $(DESTDIR)$(INCLUDEDIR)\.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(objs:.o=.d)
-include $(libk_objs:.o=.d)
