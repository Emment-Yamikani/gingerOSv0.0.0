#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>

//Convert single to ASCII upperCase sentence.
int toupper(int ch)
{
    if (isalpha(ch) && islower(ch))
        return (ch & 0x5F);
    else
        return ch;
}