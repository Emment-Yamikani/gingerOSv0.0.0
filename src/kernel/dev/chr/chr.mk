tty_dir=$(chr_dir)\tty
kb_dir=$(chr_dir)\kb

include $(kb_dir)\kb.mk
include $(tty_dir)\tty.mk

chr_objs=\
$(tty_objs)\
$(kb_objs)