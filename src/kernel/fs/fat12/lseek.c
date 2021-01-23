#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>
#include <stdio.h>

int fat12_lseek(file_t *file, off_t off, int whence)
{
    printf("fat12: lseeking file\n");
    return 0;
}