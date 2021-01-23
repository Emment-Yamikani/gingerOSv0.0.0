#include <stdint.h>
#include <arch\x86\cpu\x86io.h>
#include <sys\panic.h>
#include <arch\x86\cpu\isrs.h>

typedef void (*vect_t)(void *);

#define max_handlers 256

uint32_t int_no = 0;
uint32_t err_code = 0;
static vect_t handlers[max_handlers] = {0};
static vect_t old_handlers[max_handlers] = {0};
extern void syscall_stub(void *arch);

vect_t getvector(uint32_t vect_no)
{
    return handlers[vect_no];
}

vect_t setvector(uint32_t vect_no, vect_t vect)
{
    vect_t old_handler = handlers[vect_no];
    handlers[vect_no] = vect;
    return old_handler;
}

void install_isr_handler(uint32_t vect_no, vect_t vect)
{
    old_handlers[vect_no] = setvector(vect_no, vect);
}

static void div0_handler(void *arch)
{
    x86_dump_context(arch);
    panic("div0_handler: not implemented");
}

static void debug_handler(void *arch)
{
    x86_dump_context(arch);
    panic("debug_handler: not implemented");
}

static void breakpoint_handler(void *arch)
{
    x86_dump_context(arch);
    panic("breakpoint_handler: not implemented");
}

static void overflow_handler(void *arch)
{
    x86_dump_context(arch);
    panic("overflow_handler: not implemented");
}

static void boundscheck_handler(void *arch)
{
    x86_dump_context(arch);
    panic("boundscheck_handler: not implemented");
}

static void invalid_opcode_handler(void *arch)
{
    x86_dump_context(arch);
    panic("invalid_opcode_handler: not implemented");
}

static void nocoprocessor_handler(void *arch)
{
    x86_dump_context(arch);
    panic("nocoprocessor_handler: not implemented");
}

static void doublefault_handler(void *arch)
{
    x86_dump_context(arch);
    panic("doublefault_handler: not implemented");
}

static void coproc_segment_overrun_handler(void *arch)
{
    x86_dump_context(arch);
    panic("coproc_segment_overrun_handler: not implemented");
}

static void badtss_handler(void *arch)
{
    x86_dump_context(arch);
    panic("badtss_handler: not implemented");
}

static void segment_not_present_handler(void *arch)
{
    x86_dump_context(arch);
    panic("segment_not_present_handler: not implemented");
}

static void stack_except_handler(void *arch)
{
    x86_dump_context(arch);
    panic("stack_except_handler: not implemented");
}

static void general_protection_handler(void *arch)
{
    x86_dump_context(arch);
    panic("general_protection_handler: not implemented");
}

static void coprocessor_err_handler(void *arch)
{
    x86_dump_context(arch);
    panic("coprocessor_err_handler: not implemented");
}

static void undef_handler(void *arch)
{
    x86_dump_context(arch);
    panic("undefined exception [%d] -system haulted", int_no);
}

void install_handlers()
{
    install_isr_handler((uint32_t)div0, div0_handler);
    install_isr_handler((uint32_t)undef1, undef_handler);
    install_isr_handler((uint32_t)debug, debug_handler);
    install_isr_handler((uint32_t)breakpoint, breakpoint_handler);
    install_isr_handler((uint32_t)overflow, overflow_handler);
    install_isr_handler((uint32_t)boundscheck, boundscheck_handler);
    install_isr_handler((uint32_t)invalid_opcode, invalid_opcode_handler);
    install_isr_handler((uint32_t)nocoprocessor, nocoprocessor_handler);
    install_isr_handler((uint32_t)doublefault, doublefault_handler);
    install_isr_handler((uint32_t)coproc_segment_overrun, coproc_segment_overrun_handler);
    install_isr_handler((uint32_t)badtss, badtss_handler);
    install_isr_handler((uint32_t)segment_not_present, segment_not_present_handler);
    install_isr_handler((uint32_t)stack_except, stack_except_handler);
    install_isr_handler((uint32_t)general_protection, general_protection_handler);
    install_isr_handler((uint32_t)pagefault, undef_handler);
    install_isr_handler((uint32_t)undef15, undef_handler);
    install_isr_handler((uint32_t)coprocessor_err, coprocessor_err_handler);
    for(int i=17; i < max_handlers; ++i)
        install_isr_handler((uint32_t) i, undef_handler);
    install_isr_handler((uint32_t)128, syscall_stub);
}

void isr_handler(void *arch)
{
    int_no &= 0xff;
    if(int_no > max_handlers)
        panic("isr_handler: int_no > max_handlers");;
    vect_t handler = handlers[int_no];
    if(handler)
        handler(arch);
    else panic("isr_handler: handler is null");
}

void irq_handler(void * arch)
{
    int_no &= 0xff;
    if(int_no > max_handlers)
        panic("irq_handler: int_no > max_handlers");;
    vect_t handler = handlers[int_no];
    if(handler)
        handler(arch);
    if(int_no >=40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}