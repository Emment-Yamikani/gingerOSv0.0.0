#ifndef FS_H
# define FS_H

#include <stdint.h>
#include <stddef.h>
#include <sys/cdefs.h>


struct fs
{
    const char *name;
    int (*initfs)();
    int (*mount) ();
    
};

struct super_block
{
    dev_t       s_dev;
    size_t      s_maxbytes;
    size_t      s_blocksize;
    struct fs   *fs_type;
    int         s_count;
    uuid_t      uuid;

};

struct inode
{
    enum
    {
        _INVALID, FS_RGL, FS_DIR, FS_CHRDEV, FS_BLKDEV,
        FS_SYMLINK, FS_PIPE, FS_FIFO, FS_SOCKET, FS_SPECIAL
    }type;

};


#endif