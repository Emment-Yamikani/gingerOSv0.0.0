#include <stdint.h>

uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count)
{
    uint16_t *temp = (uint16_t *)dest;
    for (; count != 0; count--)
        *temp++ = val;
    return dest;
}