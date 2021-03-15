#ifndef MOUNT_H
# define MOUNT_H

struct dentry;
struct super_block;


struct vfsmount
{
    struct dentry *mnt_root;    /* root of the mounted tree */
    struct super_block *mnt_sb; /* pointer to superblock */
    int mnt_flags;
};

#endif