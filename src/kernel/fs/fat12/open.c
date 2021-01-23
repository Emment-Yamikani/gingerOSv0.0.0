#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>
#include <stdio.h>

int fat12_open(file_t *file)
{
    printf("fat12: opening file\n");
    return 0;
}