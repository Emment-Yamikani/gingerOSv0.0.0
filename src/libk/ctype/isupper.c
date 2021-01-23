#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>
//Checks if a a single char is uppercase.
int isupper(int ch)
{
    if (isalpha(ch) && ((ch >= ASCII_A) && (ch <= ASCII_Z)))
        return (true);
    else
    {
        return (false);
    }
}
