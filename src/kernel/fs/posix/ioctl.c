#include <system.h>
#include <fs\vfs.h>
#include <errno.h>

/**
 * posix_file_ioctl
 *
 * Conforming to `IEEE Std 1003.1, 2013 Edition'
 *
 */

int posix_file_ioctl(struct file *file, int request, void *argp)
{
    return vfs_ioctl(file->inode, request, argp);
}
