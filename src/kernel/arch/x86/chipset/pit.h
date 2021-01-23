#ifndef CHIPSET_PIT
#define CHIPSET_PIT 1

//#include <cmos.h>

#define CLK 1193181.66666666666

#include <stdint.h>

enum PIT_IO
{
    CH0_PORT = 0x40,
    CH1_PORT = 0x41,
    CH2_PORT = 0x42,
    PIT_MODE_PORT = 0x43
};

enum MODES
{
    MODE0 = 0b00000000,
    MODE1 = 0b00000010,
    MODE2 = 0b00000100,
    MODE3 = 0b00000110,
    MODE4 = 0b00001000,
    MODE5 = 0b00001010,
    BCD_BIN = 0b00000001
};

enum CH
{
    CH0 = 0b00000000,
    CH1 = 0b01000000,
    CH2 = 0b10000000,
    READ_CALLBACK = 0b11000000
};

enum ACCESS_MODES
{
    COUNT_VAL = 0b00000000,
    LO_BYTE_ONLY = 0b00010000,
    HI_BYTE_ONLY = 0b00100000,
    LO_HI_LAST = 0b00110000
};

#define I86_PIT_OCW_MASK_BINCOUNT 1        //00000001
#define I86_PIT_OCW_MASK_MODE 0xE          //00001110
#define I86_PIT_OCW_MASK_RL 0x30           //00110000
#define I86_PIT_OCW_MASK_COUNTER 0xC0      //11000000
#define I86_PIT_OCW_BINCOUNT_BINARY 0      //0 //! Use when setting I86_PIT_OCW_MASK_BINCOUNT
#define I86_PIT_OCW_BINCOUNT_BCD 1         //1
#define I86_PIT_OCW_MODE_TERMINALCOUNT 0   //0000 //! Use when setting I86_PIT_OCW_MASK_MODE
#define I86_PIT_OCW_MODE_ONESHOT 0x2       //0010
#define I86_PIT_OCW_MODE_RATEGEN 0x4       //0100
#define I86_PIT_OCW_MODE_SQUAREWAVEGEN 0x6 //0110
#define I86_PIT_OCW_MODE_SOFTWARETRIG 0x8  //1000
#define I86_PIT_OCW_MODE_HARDWARETRIG 0xA  //1010
#define I86_PIT_OCW_RL_LATCH 0             //000000 //! Use when setting I86_PIT_OCW_MASK_RL
#define I86_PIT_OCW_RL_LSBONLY 0x10        //010000
#define I86_PIT_OCW_RL_MSBONLY 0x20        //100000
#define I86_PIT_OCW_RL_DATA 0x30           //110000

#define I86_PIT_OCW_COUNTER_0 0    //00000000 //! Use when setting I86_PIT_OCW_MASK_COUNTER
#define I86_PIT_OCW_COUNTER_1 0x40 //01000000
#define I86_PIT_OCW_COUNTER_2 0x80 //10000000

#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND 0x43

//gets value of channel's counter register
uint16_t get_count(uint8_t ch);
extern uint32_t msec, sec;
extern uint32_t secx_;
extern uint32_t uptime;

#endif //_8253_TIMER_H