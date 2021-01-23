#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);

#endif