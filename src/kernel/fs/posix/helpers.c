#include <fs/posix.h>

int posix_file_can_read(struct file *file, size_t size)
{
    return (size_t) file->foffset + size < file->fnode->isize;
}

int posix_file_can_write(struct file *file, size_t size)
{
    return (size_t) file->foffset + size < file->fnode->isize;
}

int posix_file_eof(struct file *file)
{
    return (size_t) file->foffset >= file->fnode->isize;
}
