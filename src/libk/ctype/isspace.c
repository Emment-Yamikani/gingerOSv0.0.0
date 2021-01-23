#include "../include/ascii.h"
#include <stdbool.h>

int isspace(int c)
{
    if(c == ASCII_SP)
        return true;
    else
        return false;
}