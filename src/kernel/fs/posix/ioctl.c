#include <core/system.h>

#include <fs/fs.h>
#include <sys/errno.h>

/**
 * posix_file_ioctl
 *
 * Conforming to `IEEE Std 1003.1, 2013 Edition'
 *
 */

int posix_file_ioctl(struct file *file, int request, void *argp)
{
    return vfs_ioctl(file->node, request, argp);
}
