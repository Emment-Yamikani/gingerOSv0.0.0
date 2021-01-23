#include "../include/ascii.h"

//copies one str from one array to another
void strcpy(char str1[], const char *str0)
{
    int i =0;
    while((str1[i] = str0[i])) i +=1;
    str1[i] = '\0';
}

void strncpy(char str1[], const char *str0, int len)
{
    int i = 0;
    while ((str1[i] = str0[i]) && ((len--) !=0)) i += 1;
    str1[i] = '\0';
}