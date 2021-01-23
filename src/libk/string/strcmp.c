#include "../include/ascii.h"

//compares for str equality
int strcmp(const char *str1, const char *str0)
{
    int diff;
    for (int i = 0; str0[i] != '\0';)
        if (str0[i] == str1[i])
            i++;
        else
        {
            diff = str1[i] - str0[i];
            return diff;
        }
    return 0;
}