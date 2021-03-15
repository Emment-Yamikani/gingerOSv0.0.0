#ifndef INITRD_H
# define INITRD_H 1
#include <fs/fs.h>
#include <fs/fat12.h>


#define FAT_DIR 1
#define FAT_FILE 2
#define FAT_UNDEF 0

typedef struct ramfs_inode
{
    char *name;
    int type; /*file type*/
    size_t size;
    size_t offset;
    size_t relative_offset;
    size_t current_cluster;
    fat12_dirent_t *inmem_metadata;
    int cluster0; /*first fat cluster*/
    int parent_cluster0; /*parent's first cluster*/
} ramfs_inode_t;

/*bootstrap helpers*/

extern int ramfs_init();
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

extern int parseToDos(char dest[], const char src[]);

inline static void ramfs_dumpdirent(fat12_dirent_t *d)
{
    if(!d)
        return;
    kprintemphasis(pink, "name    =%s\n", d->name);
    kprintemphasis(pink, "attrib  =%x\n", d->attrib);
    kprintemphasis(pink, "_unused =%x\n", d->_unused);
    kprintemphasis(pink, "ctime   =%x\n", d->ctime);
    kprintemphasis(pink, "ctime16 =%x\n", d->ctime16);
    kprintemphasis(pink, "cdate   =%x\n", d->cdate);
    kprintemphasis(pink, "adate   =%x\n", d->adate);
    kprintemphasis(pink, "ea_index=%x\n", d->ea_index);
    kprintemphasis(pink, "mtime   =%x\n", d->mtime);
    kprintemphasis(pink, "mdate   =%x\n", d->mdate);
    kprintemphasis(pink, "cluster0=%d\n", d->first_cluster);
    kprintemphasis(pink, "size    =%dB\n", d->size);
}


#endif