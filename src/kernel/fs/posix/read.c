/*
 *          VFS => Generic file read function
 *
 *
 *  This file is part of ginger OS and is released under
 *  the terms of GNU GPLv3 - See LICENSE.
 *
 */

#include <fs/fs.h>

#include <sys/fcntl.h>
#include <sys/errno.h>
/**
 * posix_file_read
 *
 * Reads up to `size' bytes from a file to `buf'.
 * Conforming to `IEEE Std 1003.1, 2013 Edition'
 * 
 * @file    File Descriptor for the function to operate on.
 * @buf     Buffer to write to.
 * @size    Number of bytes to read.
 * @returns read bytes on success, or error-code on failure.
 */

ssize_t posix_file_read(struct file *file, void *buf, size_t size)
{
    if (file->fflags & O_WRONLY) /* File is not opened for reading */{
        kprintfailure("not valid fd\n");
        return -EBADFD;
    }
    if (!size)
        return 0;
    
    int retval =0;
    for (;;) {
        if ((retval = vfs_iread(file->fnode, file->foffset, size, buf)) > 0) {
            /* Update file offset */
            file->foffset += retval;
            
            /* Wake up all sleeping writers if a `write_queue' is attached */
            //!if (file->fnode->write_queue);
                //!thread_queue_wakeup(file->fnode->write_queue);

            /* Return read bytes count */
            return retval;
        } else if (retval < 0) {    /* Error */
            return retval;
        } else if (vfs_feof(file)) {
            /* Reached end-of-file */
            return 0;
        }//! else if (file->fflags & O_NONBLOCK) {
         //!   /* Can not satisfy read operation, would block */
         //!   return -EAGAIN;
        //!} 
        else {
            /* Block until some data is available */
            /* Sleep on the file readers queue */
            //!if (thread_queue_sleep(file->fnode->read_queue))
            //!    return -EINTR;
        }
    }
}
