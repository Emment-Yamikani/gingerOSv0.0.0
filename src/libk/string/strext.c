#include "../include/ascii.h"
#include "../include/ctype.h"
#include <stddef.h>


extern void *kmalloc(size_t);
extern void kfree(void *);
extern int strlen(const char *);
extern char *strdup(const char *s);

//extracts a short string from the entire str
void strext(char str1[], const char str0[], int offset, int len)
{
    int i =0;
    while(len--){
        str1[i] = str0[offset + i];
        i += 1;
    }
    str1[i] = '\0';
}

char *strextc(const char *str0, int c)
{
    char *str = (char *)str0;
    int len = strlen(str);
    char *str1 = 0, *str2;
    str1 = (char *)kmalloc(len + 1);
    str2 = str1;
    while (len--)
        if ((*str1++ = *str++) == (char)c)
            break;
    *--str1 = '\0';
    return str2;
}

char *strextdigits(const char *str0, int c)
{
    int len = strlen(str0);
    char *str2 = (char *)str0;
    char *str1 = (char *)kmalloc(len + 1);
    char *str = str1;
    while (len--)
    {
        if (isdigit(*str2))
            *str1++ = *str2++;
        if ((*str2++) == (char)c)
            break;
    }
    return str;
}

char **tokenize(const char *s, char c)
{
    if (!s || !*s)
        return NULL;

    while (*s == c)
        ++s;

    char *tokens = strdup(s);
    int len = strlen(s);

    if (!len)
    {
        char **ret = kmalloc(sizeof(char *));
        *ret = NULL;
        return ret;
    }

    int i, count = 0;
    for (i = 0; i < len; ++i)
    {
        if (tokens[i] == '/')
        {
            tokens[i] = 0;
            ++count;
        }
    }

    if (s[len - 1] != c)
        ++count;

    char **ret = kmalloc(sizeof(char *) * (count + 1));

    int j = 0;
    ret[j++] = tokens;

    for (i = 0; i < strlen(s) - 1; ++i)
        if (tokens[i] == 0)
            ret[j++] = &tokens[i + 1];

    ret[j] = NULL;

    return ret;
}

void free_tokens(char **ptr)
{
    if (!ptr)
        return;
    if (*ptr)
        kfree(*ptr);
    kfree(ptr);
}