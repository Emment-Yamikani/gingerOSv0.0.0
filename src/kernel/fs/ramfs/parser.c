#include <ctype.h>
#include <system.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/errno.h>
#include <mm/kmalloc.h>
#include <system.h>

int parseToDos(char *dest, const char *src)
{
    if(!dest || !src)
    return -ENOTNAM;

    if(strlen(src) >= 12)
    return -EINVAL;

    char * root = NULL, *ext = NULL;
    root = strextc(src, '.');
    strcpy(dest, root);
    memset(dest + strlen(root), ' ', (8 - strlen(root)));
    ext = (char *)src + strlen(root) + 1;
    strcpy(dest + 8, ext);
    memset(dest + 8 + strlen(ext), ' ', (3 - strlen(ext)));
    kfree(root);
    dest[11] = 0;
    char tmp[12];
    strtoupper(tmp, dest);
    tmp[11] =0;
    strcpy(dest, tmp);
    dest[11] = 0;
    return 0;
}