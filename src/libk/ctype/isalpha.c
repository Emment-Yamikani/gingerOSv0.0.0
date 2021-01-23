#include "../include/ascii.h"
#include <stdbool.h>

//Checks if a a single char is an ASCII letter.
int isalpha(int ch)
{
    if (((ch >= ASCII_A) && (ch <= ASCII_Z)) || ((ch >= ASCII_a) && (ch <= ASCII_z)))
        return (true);
    else
    {
        return (false);
    }
}
