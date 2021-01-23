#ifndef _STRING_H
#define _STRING_H 1

#include "sys/cdefs.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
//compare arrays of memory
int memcmp(const void*, const void*, size_t);
//copy memory strung to another location
void* memcpy(void* __restrict, const void* __restrict, size_t);
//move array of memory to another location
void* memmove(void*, const void*, size_t);
//set memory array to value
void* memset(void*, int, size_t);
//length of string
size_t strlen(const char*);
char *substr(const char str0[], int offset, int len);
void strext(char str1[], const char str0[], int offset, int len);
char *strdup(const char *s);
void strcpy(char str1[], const char *str0);
int strcmp(const char *str1, const char *str0);
char *strcat(char str1[], const char str0[]);
uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count);


#ifdef __cplusplus
}
#endif

#endif
