#ifndef X86_REGS_H
# define X86_REGS_H
#include <stdint.h>


extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);
extern void tss_flush();

extern uint32_t _read_cr0();
extern uint32_t _read_cr2();
extern uint32_t _read_cr3();
extern uint32_t _read_cr4();
extern uint32_t _read_esp();
extern uint32_t _read_ebp();
extern uint32_t _read_eip();

extern uint32_t _write_cr0(uint32_t);
extern uint32_t _write_cr3(uint32_t);
extern uint32_t _write_cr4(uint32_t);
extern uint32_t _write_esp(uint32_t);
extern uint32_t _write_ebp(uint32_t);

#endif