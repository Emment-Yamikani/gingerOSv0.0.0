#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>
#include <stdio.h>

int fat12_creat(file_t *file)
{
    printf("fat12: creating file\n");
    return 0;
}