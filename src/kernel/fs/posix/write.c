/*
 *			VFS => Generic file write function
 *
 *
 *	This file is part of Aquila OS and is released under
 *	the terms of GNU GPLv3 - See LICENSE.
 *
 *	Copyright (C) 2016 Mohamed Anwar <mohamed_anwar@opmbx.org>
 */

#include <fs/fs.h>

#include <sys/fcntl.h>
#include <sys/errno.h>

/**
 * posix_file_write
 *
 * Writes up to `size' bytes from `buf' into a file.
 *
 * @file 	File Descriptor for the function to operate on.
 * @buf  	Buffer to read from.
 * @size 	Number of bytes to write.
 * @returns written bytes on success, or error-code on failure.
 */

ssize_t posix_file_write(struct file *file, void *buf, size_t size)
{
	if (!(file->fflags & (O_WRONLY | O_RDWR)))	/* File is not opened for writing */
		return -EBADFD;
	
	if (file->fflags & O_NONBLOCK) {	/* Non-blocking I/O */
		if (vfs_file_can_write(file, size)) {
			/* write up to `size' from `buf' into file */
			ssize_t retval = vfs_write(file->fnode, file->foffset, size, buf);

			/* Update file offset */
			file->foffset += retval;
			
			/* Wake up all sleeping readers if a `read_queue' is attached */
			if (file->fnode->read_queue)
				thread_queue_wakeup(file->fnode->read_queue);

			/* Return written bytes count */
			return retval;
		} else {
			/* Can not satisfy write operation, would block */
			return -EAGAIN;
		}
	} else {	/* Blocking I/O */
		ssize_t retval = size;
		
		while (size) {
			size -= vfs_write(file->fnode, file->foffset, size, buf);

			/* No bytes left to be written, or reached END-OF-FILE */
			if (!size || vfs_file_eof(file))	/* Done writting */
				break;

			/* Sleep on the file writers queue */
			thread_queue_sleep(file->fnode->write_queue);
		}
		
		/* Store written bytes count */
		retval -= size;

		/* Update file offset */
		file->foffset += retval;

		/* Wake up all sleeping readers if a `read_queue' is attached */
		if (file->fnode->read_queue)
			thread_queue_wakeup(file->fnode->read_queue);

		return retval;
	}
}
