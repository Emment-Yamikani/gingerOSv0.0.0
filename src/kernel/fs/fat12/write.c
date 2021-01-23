#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>
#include <stdio.h>


int fat12_write(file_t *file, char *buf, size_t size)
{
    printf("fat12: writing file\n");
    return 0;
}