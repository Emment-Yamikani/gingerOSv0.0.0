#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>
#include <stdio.h>

int fat12_close(file_t *file)
{
    printf("fat12: closing file\n");
    return 0;
}