#include "../include/ascii.h"

static char buf[256];

//extracts a portion of a string
char *substr(const char str0[], int offset, int len)
{
    int i = 0;
    while(len--){
        buf[i] = str0[offset + i];
        i += 1;
    }
    buf[i] = '\0';
    return buf;
}
