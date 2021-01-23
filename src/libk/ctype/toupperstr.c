#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>

/*Converts a string to UpperCase
* and returns the address of the array
* of the the string.
*/
static char buf[256];
char *toupperstr(const char str0[])
{
    int i=0;
    while((buf[i] = toupper(str0[i]))) i +=1;;
    return buf;
}
