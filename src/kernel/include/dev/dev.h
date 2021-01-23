#ifndef DEV_H
# define DEV_H
#include <stdint.h>
#include <fs\vfs.h>
#include <sys\cdefs.h>


enum
{
    CHRDEV = FS_CHRDEV,
    BLKDEV = FS_BLKDEV,
};

struct devid
{
    int type;
    uint8_t major;
    uint8_t minor;
};

struct dev
{
    char   *name;
    int    (*probe)(void);
    size_t (*read)(struct devid *dev, off_t offset, size_t size, void *buf);
    size_t (*write)(struct devid *dev, off_t offset, size_t size, void *buf);
    struct fops fops;
};

/* Kernel Device Subsystem Handlers */
extern void kdev_init();
extern void kdev_chrdev_register(devid_t major, struct dev *dev);
extern void kdev_blkdev_register(devid_t major, struct dev *dev);

extern int kdev_open(struct devid *dd);
extern size_t kdev_read(struct devid *dd, off_t offset, size_t size, void *buf);
extern size_t kdev_write(struct devid *dd, off_t offset, size_t size, void *buf);
extern int kdev_close(struct devid *dd);

/* useful Macros */

#define ISDEV(inode) ((inode)->type == FS_CHRDEV || (inode)->type == FS_BLKDEV)
#define _DEV_T(major, minor) ((dev_t)(((major)&0xFF) << 8) | ((minor)&0xFF))
#define _DEV_MAJOR(dev) ((devid_t)(((dev) >> 8) & 0xFF))
#define _DEV_MINOR(dev) ((devid_t)(((dev) >> 0) & 0xFF))
#define _INODE_DEV(inode) ((struct devid){.type = (inode)->type, .major = _DEV_MAJOR((inode)->rdev), .minor = _DEV_MINOR((inode)->rdev)})

extern struct dev consdev;
extern struct dev ramdev;
extern struct dev ps2kbd;

#endif