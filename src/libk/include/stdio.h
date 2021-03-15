#ifndef _STDIO_H
#define _STDIO_H 1

#include "sys/cdefs.h"
//End of file
#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

//printf(char*, ...)
int printf(const char* __restrict, ...);
//print single char
int putchar(int);
//print a string of text
int puts(const char*);
char *int_str(unsigned int n, int space, int type, int _signed, int base);
#ifdef __cplusplus
}
#endif

#endif
