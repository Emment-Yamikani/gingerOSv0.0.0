#include "../include/ascii.h"
#include "../include/ctype.h"
#include "../include/string.h"
#include <stdbool.h>

extern void *kmalloc(unsigned int);

static char *buf;

//Convert string to ASCII lowerCase sentence.
char *tolowerstr(const char str0[]){
    int i =0;
    buf = (char *)kmalloc(strlen(str0));
    while((buf[i] = (char)tolower(str0[i])))i +=1;
    return buf;
}
