#include <fs/fs.h>
#include <sys/errno.h>

/**
 * posix_file_lseek
 *
 * Conforming to `IEEE Std 1003.1, 2013 Edition'
 *
 */

ssize_t posix_file_lseek(struct file *file, off_t offset, int whence)
{
    struct inode *inode = file->fnode;

    switch (whence) {
        case 0: /* SEEK_SET */
            file->foffset = offset;
            break;
        case 1: /* SEEK_CUR */
            file->foffset += offset;
            break;
        case 2: /* SEEK_END */
            file->foffset = inode->isize + offset;
            break;
    }

    return file->foffset;
}
