#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stdbool.h>
//Convert single to ASCII lowerCase.
int tolower(int ch)
{
    if (isalpha(ch) && isupper(ch))
        return (ch | 0x20);
    else
        return ch;
}
