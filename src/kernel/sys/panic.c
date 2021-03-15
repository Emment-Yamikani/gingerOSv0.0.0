#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <dev/console/console.h>


#define DEC 0
#define BIN 1
#define OCT 2
#define HEX 3

void panic(const char *fmt, ...)
{
    printf("\n[");
    cons_setforecolor(red);
    cons_update_color();
    printf("!PANIC!");
    cons_restore_color();
    printf("]");
    va_list list;
    va_start(list, fmt);
    kvprintf(fmt, list);
    va_end(list);
    for(;;)
    {
        asm volatile("cli");
        asm volatile("hlt");
    }
}