#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

typedef unsigned char color_t;
extern int console_write(char *buf, int size);
extern void console_clrs();
extern void cons_setcolor(color_t back, color_t fore);
extern void init_console();
extern void *kmalloc(size_t);
extern void kfree(void *);
extern void reverse(char *);
extern void *memset(void *ptr, int, unsigned int);
extern char *strcat(const char *, const char *);
extern void strcpy(char str1[], const char *str0);
extern int atoi(const char *str);
extern int isdigit(int c);

int putchar(int __c)
{
    return console_write((char *)&__c, 1);
}

int putc(int __c)
{
    return putchar(__c);
}

int puts(char *__s)
{
    int len = 0;
    while (__s[len])
        putc(__s[len++]);
    return len;
}

#define DEC 0
#define BIN 1
#define OCT 2
#define HEX 3
#define DEC_NEG 4


static char hexadecimal[32] = {0};
static char result[256] = {0};
char *int_str(unsigned int n, int space, int type, int base)
{
    memset(result, 0, sizeof(result));
    memset(hexadecimal, 0, sizeof(hexadecimal));

    if (base == BIN)
        strcpy(hexadecimal, "01");
    else if (base == OCT)
        strcpy(hexadecimal, "01234567");
    else if(base == DEC)
        strcpy(hexadecimal, "0123456789");

    int count = 0;
    switch (base)
    {
    case HEX:
        if (!type)
            strcpy(hexadecimal, "0123456789abcdef");
        else
            strcpy(hexadecimal, "0123456789ABCDEF");
        if(!n){
            result[count++] ='0';
            count++;
            break;
        }
        while (n)
        {
            result[count++] = hexadecimal[(n % 16)];
            n >>= 4;
        }
        count +=2;
        break;
    case DEC:
        if(!n){
            result[count++] ='0';
            count++;
            break;
        }
        while(n)
        {
            if (space)
                if (!(count % 4))
                    result[count++] = ',';
            result[count++] = hexadecimal[(n % 10)];
            n /= 10;
        }
        break;
    case OCT:
        if(!n){
            result[count++] ='0';
            count++;
            break;
        }
        while(n)
        {
            if (space)
                if (!(count % 4))
                    result[count++] = ' ';
            result[count++] = hexadecimal[(n % 8)];
            n >>= 3;
        }
        break;
    case BIN:
        if(!n){
            result[count++] ='0';
            count++;
            break;
        }
        while(n)
        {
            if (space)
                if (!(count % 5))
                    result[count++] = ' ';
            result[count++] = hexadecimal[n & 1];
            n >>= 1;
        }
        count++;
        break;
    }
    reverse(result);
    result[--count] = '\0';
    return result;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    char *p;
    char *str;
    unsigned int _int = 0;
    int len = 0;
    va_start(ap, fmt);
    for (p = (char *)fmt; *p; ++p)
    {
        if(*p == '\e'){
            color_t back = 0, fore = 0;
            char *s = (char *)kmalloc(64);
            switch (*++p)
            {
            case '[':
                ++p;
                int i = 0;
                while (*p)
                {
                    if (isdigit(*p))
                        s[i++] = *p++;
                    if ((*p++) == ';')
                        break;
                }
                s[i] = '\0';
                back = (color_t)atoi(s);
                i = 0;
                while (*p)
                {
                    if (isdigit(*p))
                        s[i++] = *p++;
                    if ((*p++) == 'm')
                        break;
                }
                s[i] = '\0';
                fore = (color_t)atoi(s);
                kfree((void *)s);
                cons_setcolor(back, fore);
                break;
            }
        }
        if (*p != '%')
        {
            putchar(*p);
            ++len;
            continue;
        }
        switch (*++p)
        {
        case 'b':
            _int = va_arg(ap, unsigned int);
            str = int_str(_int, 1, 0, BIN);
            len += puts(str);
            break;
        case 'D':
        case 'd':
            _int = va_arg(ap, unsigned int);
            str = int_str(_int, 1, 0, DEC);
            len += puts(str);
            break;
        case 'f':
            //float f = va_arg(ap, float);
            //str = ftoa(f);
            //puts(str);
            break;
        case 'o':
            _int = va_arg(ap, unsigned int);
            str = int_str(_int, 1, 0, OCT);
            len += puts(str);
            break;
        case 'p':
        case 'x':
            _int = va_arg(ap, unsigned int);
            str = int_str(_int, 0, 0, HEX);
            len += puts(str);
            break;
        case 'X':
            _int = va_arg(ap, unsigned int);
            str = int_str(_int, 0, 1, HEX);
            len += puts(str);
            break;
        case 's':
            str = va_arg(ap, char *);
            len += puts(str);
            break;
        case 'c':
        default:
            _int = va_arg(ap, unsigned int);
            putchar((int)_int);
            ++len;
            break;
        }
    }
    va_end(ap);
    return len;
}