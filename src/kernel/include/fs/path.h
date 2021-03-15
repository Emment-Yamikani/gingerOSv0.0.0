#ifndef PATH_H
# define PATH_H

#include <fs/dcache.h>
#include <fs/mount.h>


struct vfsmount;

struct path
{
    struct vfsmount *mnt;
    struct dentry *dentry;
};

#endif