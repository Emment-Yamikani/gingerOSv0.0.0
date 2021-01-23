#ifndef _STDIO_H
#define _STDIO_H 1
#include <stdarg.h>
#include "sys/cdefs.h"
//End of file
#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int kvprintf(const char *fmt, va_list list);
int printf(const char* __restrict, ...);
//print single char
int putchar(int);
//print a string of text
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
