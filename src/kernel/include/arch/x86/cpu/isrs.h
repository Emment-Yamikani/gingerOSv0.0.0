#ifndef CPU_ISRS_H
# define CPU_ISRS_H
#include <stdint.h>
typedef void (*vect_t)(void *);

enum faults
{
    div0,
    undef1,
    debug,
    breakpoint,
    overflow,
    boundscheck,
    invalid_opcode,
    nocoprocessor,
    doublefault,
    coproc_segment_overrun,
    badtss,
    segment_not_present,
    stack_except,
    general_protection,
    pagefault,
    undef15,
    coprocessor_err
};

enum INTERRUPTS
{
    irq0_Timer = 32,
    irq1_Keyboard = 33,
    irq2_Never = 34,
    irq3_COM2 = 35,
    irq4_COM1 = 36,
    irq5_LPT2 = 37,
    irq6_Floppy = 38,
    irq7_LPT1 = 39,
    irq8_CMOS = 40,
    irq9_Free_for_peripherals0 = 41,
    irq10_Free_for_peripherals1 = 42,
    irq11_Free_for_peripherals2 = 43,
    irq12_PS2_Mouse = 44,
    irq13_FPU_Coprocessor = 45,
    irq14_Primary_ATA_HDD = 46,
    irq15_Secondary_ATA_HDD = 47
};

extern uint32_t int_no;
extern uint32_t err_code;

extern vect_t getvector(uint32_t vect_no);
extern vect_t setvector(uint32_t vect_no, vect_t vect);
extern void install_isr_handler(uint32_t vect_no, vect_t vect);
extern void x86_dump_context(void *arch);

#endif