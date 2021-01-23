#include "../include/ascii.h"
#include "../include/string.h"


char *strcat(char str1[], const char str0[])
{

    //+1 because of the space char in the func
    int offset = strlen(str1);
    int i = 0;
    for (i = 0; str0[i]; i++)
        str1[offset + i] = str0[i];
    str1[i] = '\0';
    return str1;
}