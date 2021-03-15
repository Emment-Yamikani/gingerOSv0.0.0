#include <fs/posix.h>

int posix_file_close(struct file *file)
{
    vfs_iclose(file->fnode);
    return 0;
}
