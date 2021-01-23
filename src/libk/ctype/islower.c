#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>
//Checks if a a single char is lowercase
int islower(int ch)
{
    if ((isalpha(ch) && ((ch >= ASCII_a)) && (ch <= ASCII_z)))
        return (true);
    else
    {
        return (false);
    }
}