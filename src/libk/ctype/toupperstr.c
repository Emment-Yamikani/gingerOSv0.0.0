#include "../include/ascii.h"
#include "../include/ctype.h"
#include "../include/string.h"
#include <stdbool.h>

/*Converts a string to UpperCase
* and returns the address of the array
* of the the string.
*/
extern void *kmalloc(unsigned int);

static char *buf;
char *toupperstr(const char str0[])
{
    int i=0;
    buf = (char *)kmalloc(strlen(str0));
    while((buf[i] = toupper(str0[i]))) i +=1;;
    return buf;
}

char *strtoupper(char *str1, const char *str0)
{
    int i = 0;
    while ((str1[i] = toupper(str0[i])))
        i += 1;
    ;
    return str1;
}
