#ifndef INITRD_H
# define INITRD_H 1
#include <fs\vfs.h>

#define RAMFS_INODE_TYPE_FIFO FS_FIFO
#define RAMFS_INODE_TYPE_CHR FS_CHRDEV
#define RAMFS_INODE_TYPE_BLK FS_BLKDEV
#define RAMFS_INODE_TYPE_DIR FS_DIR
#define RAMFS_INODE_TYPE_RGL FS_RGL
#define RAMFS_INODE_TYPE_SLINK FS_SYMLINK
#define RAMFS_INODE_TYPE_SCKT FS_SOCKET

/*bootstrap helpers*/

extern void ramfs_init();
extern void load_ramdisk();
extern void initramfs_archiver_register(struct fs *fs);

/*file operations*/

extern int ramfs_open(file_t *file);
extern size_t ramfs_read(file_t *file, void *buf, size_t size);
extern size_t ramfs_write(file_t *file, void *buf, size_t size);

/*inode operations*/

extern int ramfs_close(file_t *file);
extern int ramfs_iclose(inode_t *inode);
extern size_t ramfs_iread(inode_t *inode, off_t off, size_t size, void *buf);
extern size_t ramfs_iwrite(inode_t *inode, off_t off, size_t size, void *buf);

#endif