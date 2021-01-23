#include <stddef.h>
#include "..\include\string.h"


extern void *kmalloc(size_t sz);
char *strdup(const char *s)
{
    char * str = (char *)kmalloc(strlen(s) + 1);
    strcpy(str, s);
    return str;
}