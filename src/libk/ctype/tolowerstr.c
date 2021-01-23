#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>
static char buf[256];
//Convert string to ASCII lowerCase sentence.
char *tolowerstr(const char str0[]){
    int i =0;
    while((buf[i] = (char)tolower(str0[i])))i +=1;
    return buf;
}
