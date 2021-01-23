#include <stdio.h>
#include <string.h>
#include <system.h>
#include <fs\vfs.h>
#include <fs\itbl.h>
#include <fs\itbl.h>
#include <sys\usr.h>
#include <sys\usr.h>
#include <sys\errno.h>
#include <sys\panic.h>
#include <boot\boot.h>
#include <mm\kmalloc.h>
#include <core\module.h>
#include <fs\ramfs\ramfs.h>
#include <fs\fat12\fat12.h>

fs_t ramfs;
inode_t *ramfs_root = NULL;
static bs_t *bs;
static struct __ramfs
{
    struct itbl  *itbl;
    struct _bs   *bootsector;
    struct inode *supernode;
    size_t        fatsz;
    uint16_t      *fat1;
    uint16_t      *fat0;
    size_t        blocksz;
    uint16_t      fat0_off;
    uint16_t      fat1_off;
    uintptr_t     root_off;
}ramfs_desc;

extern void addnam();
void ramfs_init()
{
    vfs_registerfs(&ramfs);
    initramfs_archiver_register(&ramfs);
}

static fat12_dirent_t *ramfs_inode_read(struct __ramfs *desc __unused, const char *path __unused)
{
    return (fat12_dirent_t *)0;
}

static int ramfs_load(inode_t *dev, inode_t **super)
{
    bs = (bs_t *)kmalloc(512);
    if(!bs)
        return -ENOMEM;
    memset((void *)bs, 0, 512);
    vfs_iread(dev, 0, 512, (void *)bs);
    
    if( strcmp((char *)bs->filesys, "FAT12"))
    {
        kprintfailure("%s not FAT12\n", bs->filesys);
        kfree((void *)bs);
        return -EINVAL;
    }
    
    memset((void *)&ramfs_desc, 0, sizeof(ramfs_desc));

    ramfs_desc.supernode = dev;
    ramfs_desc.bootsector = bs;
    ramfs_desc.blocksz = bs->sects_per_cluster * bs->bytes_per_sect;
    ramfs_desc.fat0_off = bs->reserved_sects * bs->bytes_per_sect;
    ramfs_desc.fat1_off = (bs->fat_sz + bs->reserved_sects) * bs->bytes_per_sect;
    ramfs_desc.fatsz = bs->fat_sz * bs->bytes_per_sect;
    ramfs_desc.fat0 = (uint16_t *)kmalloc(ramfs_desc.fatsz);
    ramfs_desc.fat1 = 0;

    vfs_iread(ramfs_desc.supernode, ramfs_desc.fat0_off, ramfs_desc.fatsz, ramfs_desc.fat0);
    if(super){
        inode_t *node = (inode_t *)kmalloc(sizeof(inode_t));
        *node = (inode_t){
            .flags = 0777,
            .fs = &ramfs,
            .gid = cur_usr->gid,
            .id = 0,
            .mask = 0777,
            .name = cur_usr->cwd,
            .nlinks = 1,
            .p = &ramfs_desc,
            .rdev = 0,
            .ref = 1,
            .size = -1, //Mb
            .type = FS_DIR,
            .uid = cur_usr->uid,
        };
        *super = node;
    }
    return 0;
}

static int ramfs_mount(const char *dir __unused, int flags __unused, void *data __unused)
{
    return 0;
}

/* file operations*/

int ramfs_open(file_t *file)
{
    fat12_dirent_t *i = (fat12_dirent_t *)NULL;
    i = ramfs_inode_read(&ramfs_desc, file->inode->name);
    uint32_t block = i->first_cluster;

    if (block && 1)
        block >>= 4;
    else
        block &= 0xfff;
    return block;
}

size_t ramfs_read(file_t *file, void *buf, size_t size)
{
    return ramfs_iread(file->inode, file->offset, size, buf);
}

size_t ramfs_write(file_t *file, void *buf, size_t size)
{
    return ramfs_iwrite(file->inode, file->offset, size, buf);
}

int ramfs_close(file_t *file)
{
    return ramfs_iclose(file->inode);
}

/*inode operations*/

size_t ramfs_iread(inode_t *inode __unused, off_t offset __unused, size_t size __unused, void *buf __unused)
{
    for(int i=0; i < 16; ++i){
        vfs_iread(ramfs_desc.supernode, ramfs_desc.root_off, ramfs_desc.blocksz, buf);
    }
    return -EINVAL;
}

size_t ramfs_iwrite(inode_t *inode __unused, off_t offset __unused, size_t size __unused, void *buf __unused)
{
    return -EINVAL;
}

int ramfs_iclose(inode_t *inode __unused)
{
    return -EINVAL;
}

int ramfs_vget(struct vnode *vnode __unused, struct inode **inode __unused)
{
    return -EINVAL;
}

int ramfs_vfind(struct vnode *parent, const char *name, struct vnode *child)
{
    if (parent->type != FS_DIR)
        return -ENOTDIR;
    return -EINVAL;
}


fs_t ramfs = {
    .name = "ramfs",
    .load = ramfs_load,
    .mount = ramfs_mount,
    /*file operations*/
    .fops = {
        .open = ramfs_open,
        .read = ramfs_read,
        .write = ramfs_write,
        .close = ramfs_close
    },
    /*inode operations*/
    .iops = {
        .iclose = ramfs_iclose,
        .iread = ramfs_iread,
        .iwrite = ramfs_iwrite,
        .vfind = ramfs_vfind,
        .vget = ramfs_vget
    }
};

MODULE_INIT(ramfs, ramfs_init, NULL)