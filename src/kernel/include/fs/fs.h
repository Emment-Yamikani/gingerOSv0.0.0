#ifndef FS_H
# define FS_H

#include <stdint.h>
#include <stddef.h>
#include <fs/path.h>
#include <ds/hash.h>
#include <ds/queue.h>
#include <sys/cdefs.h>

typedef enum
{
    INVAL,
    RGL,
    DIR,
    CHRDEV,
    BLKDEV,
    SYMLINK,
    SPECIAL
} ftype_t;

struct inode;
typedef struct inode inode_t;
struct sops;


typedef struct super_block
{
    dev_t       sdev; /*device association*/
    uint8_t     uuid[16];
    int         scount;
    uint32_t    sflags;
    dentry_t    *sroot;
    uint32_t    smagic;
    void        *sfsinfo;
    size_t      smaxbytes; /*max file size*/
    size_t      sblocksize;
    struct fs   *sfstype;
    size_t      ssize;
    //struct sops sops; /*operations on this super_block*/
} super_block_t;

typedef struct inode
{
    mode_t      imode;  /*mode of access*/
    id_t        iuid;   /*owner*/
    id_t        igid;   /*owner group*/
    ftype_t     itype;  /*object type*/
    uint32_t    iflags; /*inode flags*/

    uintptr_t   iino;   /*inode unique identifier*/
    super_block_t *isb; /*filesystem metadata*/

    uint32_t    inlinks;/*No. of hardlinks*/
    uint32_t    iref;   /*refresence count*/
    int         irdev;
    size_t      isize;  /*size of inode*/

    uint32_t    ictime; /*creation time*/
    uint32_t    imtime; /*last modify time*/
    uint32_t    iatime; /*last access time*/

    void        *iprivate; /*filesystem private data*/
    /* inode flags*/

    int hidden  :1;  /*is hidden?*/
    int locked  :1;  /*is locked?*/
    int system  :1;  /*is sysfile?*/
    int archive :1; /*needs backing up?*/
} inode_t;

#define FD_VALID    0b100000000
#define FD_LOCKED   0b1000000000

typedef struct file
{
    const char  *fname;  //file name
    struct path *fpath;  //file path
    mode_t      fmode;   //access modes on this file
    uint32_t    fflags;  //file flags
    inode_t     *fnode;  //inode associated with this file
    off_t       foffset; //rw offset
} file_t;

/*==========================================================================*/
//*structures defining the operations on files, and super blocks
/*==========================================================================*/

typedef struct iops
{
    int (*iclose)   (inode_t *inode);
    int (*iopen)    (inode_t *inode, mode_t mode);
    size_t (*iread) (inode_t *inode, off_t offset, size_t size, void *buf);
    size_t (*iwrite)(inode_t *inode, off_t offset, size_t size, void *buf);
    int (*iseek)    (inode_t *inode, off_t offset, int whence);
    int (*ifind)    (dentry_t *preant, const char *name, dentry_t **child);
} iops_t;

typedef struct fops
{
    int (*flseek)   (file_t *file, off_t offest, int whence);
    int (*fclose)   (file_t *file);
    int (*fopen)    (file_t *file);
    int (*fcan_read)(file_t *file, size_t size);
    int (*fcan_write)(file_t *file, size_t size);
    size_t (*fread) (file_t *file, void *buf, size_t size);
    size_t (*fwrite)(file_t *file, void *buf, size_t size);
    int (*eof)(file_t *file);
}fops_t;

typedef struct sops
{
    inode_t * (*alloc_inode)(super_block_t *sb);
    int (*write_inode)(inode_t *inode);
    void (*dirty_inode)(inode_t *inode);
    void (*destroy_inode)(inode_t * inode);
    int (*statfs)(dentry_t *dentry);
    int (*sync_fs)(super_block_t *sb);
}sops_t;

typedef struct fs
{
    const char *fs_name;//fs name provided by fs bootstrap code
    dentry_t *(*fs_mount)(struct fs *fs, int flags, const char *mnt); //kernel calls this to mount root dir
    int        (*fs_load)(inode_t *idev, dentry_t **droot); //kernel call to load fs superblock
    int        (*fs_kill_sb)  (super_block_t *sb);
    fops_t     fops;   //valid operations on a file
    iops_t     iops;   //valid operations on an inode
    queue_t    fs_sbqueue; /*list of all super_blocks of this fs type*/
}fs_t;

typedef struct fs_list
{
    const char *name;   /*fs name*/
    fs_t       *type;     /*fs type*/
    fs_t       *next;   /*next fs in list*/
}fs_list_t;


extern dentry_t *vroot;
/*list of registered filesystems*/
extern struct fs_list *registered_fs;

/*initialize vfs*/
extern void vfs_init();

/*refister filesystem*/
extern int vfs_registerfs(fs_t *fs_t);

/*flush vfs root*/
extern int vfs_mount_root(dentry_t *root);

/*populate directory contents*/
extern int vfs_populate_dir(dentry_t *dentry);

/*         *****            */
/*      VFS HELPERS         */
/*         *****            */

extern struct path *vfs_get_mountpoint();;
extern int vfs_parse_path(const char *path, const char *cwd, const char **abs_path);
extern int vfs_lookup(const char *path, dentry_t **dentry);

extern int vfs_find(dentry_t *parent, const char *name, dentry_t **child);

extern int vfs_fcan_read(struct file *file, size_t size);
extern int vfs_fcan_write(struct file *file, size_t size);
extern int vfs_feof(struct file *file);

/* ==================================== */
/*!   generic inode operations mapping  */
/* ==================================== */

extern size_t vfs_iread(inode_t *inode, size_t offset, size_t size, void *buf);
extern size_t vfs_iwrite(inode_t *inode, size_t offset, size_t size, void *buf);
extern int vfs_iclose(inode_t *node);

/* ==================================== */
/*!   generic file operations mapping   */
/* ==================================== */

extern int vfs_fopen(struct file *file);
extern size_t vfs_fread(file_t *file, void *buf, size_t size);
extern size_t vfs_fwrite(struct file *file, void *buf, size_t nbytes);
extern off_t vfs_flseek(struct file *file, off_t offset, int whence);
extern int vfs_fclose(struct file *file);

#endif