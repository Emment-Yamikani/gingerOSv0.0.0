#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <system.h>
#include <stdbool.h>
#include<dev/console/console.h>


int kvprintf(const char *fmt, va_list list)
{
    char *p;
    char *str;
    unsigned int _int = 0;
    int len = 0;
    for (p = (char *)fmt; *p; ++p)
    {
        if (*p != '%')
        {
            putchar(*p);
            ++len;
            continue;
        }
        switch (*++p)
        {
        case 'b':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 1, 0, false, BIN);
            len += puts(str);
            break;
        case 'd':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 1, 0, true, DEC);
            len += puts(str);
            break;
        case 'D':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 1, 0, false, DEC);
            len += puts(str);
            break;
        case 'f':
            //float f = va_arg(list, float);
            //str = ftoa(f);
            //puts(str);
            break;
        case 'o':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 1, 0, false, OCT);
            len += puts(str);
            break;
        case 'p':
        case 'x':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 0, 0, false, HEX);
            len += puts(str);
            break;
        case 'X':
            _int = va_arg(list, unsigned int);
            str = int_str(_int, 0, 1, false, HEX);
            len += puts(str);
            break;
        case 's':
            str = va_arg(list, char *);
            len += puts(str);
            break;
        case 'c':
        default:
            _int = va_arg(list, unsigned int);
            putchar((int)_int);
            ++len;
            break;
        }
    }
    return len;
}

void _KprintOK()
{
    putchar('[');
    cons_setforecolor(lgreen);
    cons_update_color();
    printf(" OK ");
    cons_restore_color();
    printf("] ");
}

void _Kprintfailure()
{
    putchar('[');
    cons_setforecolor(red);
    cons_update_color();
    printf(" FAILED ");
    cons_restore_color();
    printf("] ");
}

int kprintfOKmsg(const char *fmt, ...)
{
    _KprintOK();
    va_list list;
    va_start(list, fmt);
    int len = kvprintf(fmt, list);
    va_end(list);
    return len;
}

int kprintfailure(const char *fmt, ...)
{
    _Kprintfailure();
    va_list list;
    va_start(list, fmt);
    int len = kvprintf(fmt, list);
    va_end(list);
    return len;
}

void kprintprompt(const char *usr, const char *cwd)
{
    printf("\n[");
    cons_setforecolor(cyan);
    cons_update_color();
    printf(usr);
    cons_restore_color();
    printf("@%s-%s ", OSNAME, OSVERSION);
    cons_setforecolor(lcyan);
    cons_update_color();
    printf(cwd);
    cons_restore_color();
    printf("]$ ");
}

char **canonicalize_path(const char *const path)
{
    /* Tokenize slash seperated words in path into tokens */
    char **tokens = tokenize(path, '/');
    return tokens;
}

int kprintemphasis(color_t color, const char *fmt, ...)
{
    cons_setforecolor(color);
    cons_update_color();
    va_list list;
    va_start(list, fmt);
    int len = kvprintf(fmt, list);
    va_end(list);
    cons_restore_color();
    return len;
}