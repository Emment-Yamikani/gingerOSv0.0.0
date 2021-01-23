#include "../include/ascii.h"
#include <stdbool.h>
//Checks if a single char is an ASCII digit.
int isdigit(int ch)
{
    if (((ch >= ASCII_ZERO) )&& ((ch <= ASCII_NINE)))
        return (true);
    else
    {
        return (false);
    }
}
