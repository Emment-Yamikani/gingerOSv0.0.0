#ifndef DEV_H
# define DEV_H
#include <stdint.h>
#include <fs/fs.h>
#include <sys/cdefs.h>


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
extern int kdev_close(struct devid *dd);
extern size_t kdev_write(struct devid *dd, off_t offset, size_t size, void *buf);

extern off_t kdev_file_lseek(struct devid *dd, struct file *file, off_t offset, int whence);
extern ssize_t kdev_file_close(struct devid *dd, struct file *file);
extern int kdev_file_can_read(struct devid *dd, struct file *file, size_t size);
extern int kdev_file_can_write(struct devid *dd, struct file *file, size_t size);
extern int kdev_file_eof(struct devid *dd, struct file *file);
/* useful Macros */

#define ISDEV(inode) ((inode)->itype == CHRDEV || (inode)->itype == BLKDEV)
#define _DEV_T(major, minor) ((dev_t)(((major)&0xFF) << 8) | ((minor)&0xFF))
#define _DEV_MAJOR(dev) ((devid_t)(((dev) >> 8) & 0xFF))
#define _DEV_MINOR(dev) ((devid_t)(((dev) >> 0) & 0xFF))
#define _INODE_DEV(inode) ((struct devid){.type = (inode)->itype, .major = _DEV_MAJOR((inode)->irdev), .minor = _DEV_MINOR((inode)->irdev)})

extern struct dev consdev;
extern struct dev ramdev;
extern struct dev ps2kbd;

#endif