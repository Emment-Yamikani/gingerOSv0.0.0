#include <stdint.h>
#include <stddef.h>
#include <arch\x86\cpu\x86io.h>
#include <arch\x86\cpu\regs.h>
#include "..\include\cpu\vectors.h"
#include <string.h>


struct _gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));

static struct
{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed))gdt_ptr;

struct _idt_entry
{
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
}__attribute__((packed));

static struct
{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed))idt_ptr;

static struct
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
}__attribute__((packed))tss;

#define gdt_entries 6
#define idt_entries 256

static struct _gdt_entry gdt[gdt_entries];
static struct _idt_entry idt[idt_entries];

static void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags | 0x60;
}

static void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = base + sizeof(tss);
    gdt_set_gate(num, base, limit, 0xE9, 0x00);
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = ss0;
    tss.esp0 = esp0;
    tss.cs = 0x0b;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
}

static void init_gdt()
{
    gdt_ptr.limit = (sizeof(struct _gdt_entry) * 6) - 1;
    gdt_ptr.base = (((uint32_t)&gdt));

    gdt_set_gate(0, 0, 0, 0, 0);               
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    write_tss(5, 0x10, 0x0);
    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}

static void init_idt()
{
    idt_ptr.limit = sizeof(struct _idt_entry) * 256 - 1;
    idt_ptr.base = (((uint32_t)&idt));
    memset(&idt, 0, sizeof(struct _idt_entry) * 256);

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate(0, ((uint32_t)isr0), 0x08, 0x8E);
    idt_set_gate(1, ((uint32_t)isr1), 0x08, 0x8E);
    idt_set_gate(2, ((uint32_t)isr2), 0x08, 0x8E);
    idt_set_gate(3, ((uint32_t)isr3), 0x08, 0x8E);
    idt_set_gate(4, ((uint32_t)isr4), 0x08, 0x8E);
    idt_set_gate(5, ((uint32_t)isr5), 0x08, 0x8E);
    idt_set_gate(6, ((uint32_t)isr6), 0x08, 0x8E);
    idt_set_gate(7, ((uint32_t)isr7), 0x08, 0x8E);
    idt_set_gate(8, ((uint32_t)isr8), 0x08, 0x8E);
    idt_set_gate(9, ((uint32_t)isr9), 0x08, 0x8E);
    idt_set_gate(10, ((uint32_t)isr10), 0x08, 0x8E);
    idt_set_gate(11, ((uint32_t)isr11), 0x08, 0x8E);
    idt_set_gate(12, ((uint32_t)isr12), 0x08, 0x8E);
    idt_set_gate(13, ((uint32_t)isr13), 0x08, 0x8E);
    idt_set_gate(14, ((uint32_t)isr14), 0x08, 0x8E);
    idt_set_gate(15, ((uint32_t)isr15), 0x08, 0x8E);
    idt_set_gate(16, ((uint32_t)isr16), 0x08, 0x8E);
    idt_set_gate(17, ((uint32_t)isr17), 0x08, 0x8E);
    idt_set_gate(18, ((uint32_t)isr18), 0x08, 0x8E);
    idt_set_gate(19, ((uint32_t)isr19), 0x08, 0x8E);
    idt_set_gate(20, ((uint32_t)isr20), 0x08, 0x8E);
    idt_set_gate(21, ((uint32_t)isr21), 0x08, 0x8E);
    idt_set_gate(22, ((uint32_t)isr22), 0x08, 0x8E);
    idt_set_gate(23, ((uint32_t)isr23), 0x08, 0x8E);
    idt_set_gate(24, ((uint32_t)isr24), 0x08, 0x8E);
    idt_set_gate(25, ((uint32_t)isr25), 0x08, 0x8E);
    idt_set_gate(26, ((uint32_t)isr26), 0x08, 0x8E);
    idt_set_gate(27, ((uint32_t)isr27), 0x08, 0x8E);
    idt_set_gate(28, ((uint32_t)isr28), 0x08, 0x8E);
    idt_set_gate(29, ((uint32_t)isr29), 0x08, 0x8E);
    idt_set_gate(30, ((uint32_t)isr30), 0x08, 0x8E);
    idt_set_gate(31, ((uint32_t)isr31), 0x08, 0x8E);

    idt_set_gate(32, ((uint32_t)irq0), 0x08, 0x8E);
    idt_set_gate(33, ((uint32_t)irq1), 0x08, 0x8E);
    idt_set_gate(34, ((uint32_t)irq2), 0x08, 0x8E);
    idt_set_gate(35, ((uint32_t)irq3), 0x08, 0x8E);
    idt_set_gate(36, ((uint32_t)irq4), 0x08, 0x8E);
    idt_set_gate(37, ((uint32_t)irq5), 0x08, 0x8E);
    idt_set_gate(38, ((uint32_t)irq6), 0x08, 0x8E);
    idt_set_gate(39, ((uint32_t)irq7), 0x08, 0x8E);
    idt_set_gate(40, ((uint32_t)irq8), 0x08, 0x8E);
    idt_set_gate(41, ((uint32_t)irq9), 0x08, 0x8E);
    idt_set_gate(42, ((uint32_t)irq10), 0x08, 0x8E);
    idt_set_gate(43, ((uint32_t)irq11), 0x08, 0x8E);
    idt_set_gate(44, ((uint32_t)irq12), 0x08, 0x8E);
    idt_set_gate(45, ((uint32_t)irq13), 0x08, 0x8E);
    idt_set_gate(46, ((uint32_t)irq14), 0x08, 0x8E);
    idt_set_gate(128, ((uint32_t)isr128), 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr);
}

void init_desc()
{
    init_gdt();
    init_idt();
}


void set_kernel_stack(uint32_t kstack)
{
    tss.esp0 = kstack;
}