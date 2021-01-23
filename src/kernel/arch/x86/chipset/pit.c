#include "pit.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <arch\x86\cpu\isrs.h>
#include <arch\x86\cpu\x86io.h>
#include <system.h>


static uint8_t ocw = 0;
static size_t timer_ticks = 0;
static uint16_t frequency = 0;

static void i86_pit_send_cmd(uint8_t cmd)
{
    outb(I86_PIT_REG_COMMAND, cmd);
}

static void i86_pit_send_data(uint8_t data, uint8_t counter)
{
    uint8_t port = (counter == I86_PIT_REG_COUNTER0) ? I86_PIT_REG_COUNTER0 : (counter == I86_PIT_REG_COUNTER1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2;
    ;
    outb(port, data);
}

static void i86_pit_start_counter(uint16_t freq, uint8_t counter, uint8_t mode)
{
    if (freq == 0)
        return;

    uint16_t divisor = CLK / freq;
    ocw = 0;
    ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_LSBONLY;
    ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
    i86_pit_send_cmd(ocw);
    //! set frequency rate
    i86_pit_send_data(divisor & 0xff, counter);

    ocw = 0;
    ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_MSBONLY;
    ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
    i86_pit_send_cmd(ocw);
    i86_pit_send_data((divisor >> 8) & 0xff, counter);
    //! reset tick count
    timer_ticks = 0;
}

static void timer_handler(void *arch __unused)
{
    timer_ticks++;
}

//!init 8253
void init_8253()
{
    frequency = 100; //Hz;
    i86_pit_start_counter(frequency, I86_PIT_REG_COUNTER0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
    install_isr_handler(irq0_Timer, timer_handler);
}