#ifndef VFS_H
# define VFS_H
#include "inode.h"
#include <stddef.h>
#include <stdint.h>
#include <sys\cdefs.h>

typedef struct fs fs_t;


typedef struct file{
    inode_t *inode;
    int flags;
    off_t offset;
}file_t;

/*file operations*/
struct fops{
    int (*open)    (file_t *file);
    size_t (*read)     (file_t *file, void *buf, size_t size);
    size_t (*write)    (file_t *file, void *buf, size_t size);
    int (*close)    (file_t *file);
};

/*inode operatons*/
struct iops{
    size_t (*iread)    (inode_t *inode, off_t offset, size_t size, void *buf);
    size_t (*iwrite)   (inode_t *inode, off_t offset, size_t size, void *buf);
    int (*iclose)   (inode_t *inode);
    int (*vfind)    (vnode_t *parent, const char *name, vnode_t *child);
    int (*vget)     (vnode_t *vnode, inode_t * inode);
};

typedef struct fs{
    char *name; //type of filesystem
    int (*load)(inode_t *dev, inode_t **super); // load the file system structure
    int (*mount)(const char *dir, int flags, void *data); //mount filesystem root directory
    struct fops fops; //file operations
    struct iops iops; //inode operatons
}fs_t;

typedef struct fs_list{
    fs_t *fs;
    int ready:1; //ready for use?
    struct fs_list *next;
}fs_list_t;

typedef struct vfs_path
{
    inode_t *mountpoint;
    char **tokens;
}vfs_path_t;

extern fs_list_t *registered_fs;


extern void vfs_init();

/* Filesystem operations */
extern void vfs_registerfs(fs_t *fs);
extern int vfs_bind(const char *path, inode_t *target);
extern int vfs_mount_root(inode_t *inode);
extern int vfs_mount(const char *type, const char *dir, int flags, inode_t *inode);
extern int vfs_unmount(const char *type, const char *dir, inode_t *inode);

/* Path resolution and lookup */
extern int vfs_parse_path(const char *path, const char **abs_path);
extern int vfs_lookup(const char *path, vnode_t *vnode, const char **abs_path);
extern int vfs_vget(struct vnode *vnode, struct inode **inode);
extern int vfs_vfind(struct vnode *parent, const char *name, struct vnode *child);

/*file operations*/
extern int vfs_fopen(file_t *file);
extern size_t vfs_fread(file_t *file, void *buf, size_t size);
extern size_t vfs_fwrite(file_t *file, void *buf, size_t size);
extern int vfs_fclose(file_t *file);

/* inode operations mappings */
extern size_t vfs_iread(inode_t *inode, off_t offset, size_t size, void *buf);
extern size_t vfs_iwrite(inode_t *inode, off_t offset, size_t size, void *buf);
extern int vfs_iclose(inode_t *inode);

/* Higher level functions */
extern int vfs_creat(const char *path, inode_t **ref);
extern int vfs_mkdir(const char *path, inode_t **ref);
extern int vfs_mknod(const char *path, itype_t type, dev_t dev, inode_t **ref);
extern int vfs_unlink(const char *path);


#endif