#include <system.h>
#include <mm\kmalloc.h>
#include <fs\vfs.h>
#include <fs\fat12\fat12.h>
#include <string.h>
#include <sys\errno.h>
#include <boot\boot.h>
#include <stdio.h>


fs_t *fat12 = (fs_t *)NULL;
static bs_t *bs;
static uint16_t *FAT0 = (uint16_t *)NULL;
static uint16_t *FAT1 = (uint16_t *)NULL;


void fat12_init()
{
    kprintfOKmsg("initializing fat12 filesystem driver...\n");
    fat12 = (fs_t *)kmalloc(sizeof(fs_t));
    
    fat12->name = strdup("fat12");
    fat12->load = fat12_load;
    fat12->mount = fat12_mount;

    fat12->fops.close = fat12_close;
    fat12->fops.open = fat12_open;
    fat12->fops.read = fat12_read;
    fat12->fops.write = fat12_write;

    fat12->iops.iread = fat12_iread;
    fat12->iops.iwrite = fat12_iwrite;
    fat12->iops.iclose = fat12_iclose;
    vfs_registerfs(fat12);
}


int fat12_load(inode_t *dev, inode_t **super)
{
    bs = (bs_t *)kmalloc(sizeof(*bs));
    kprintfOKmsg("loading fat12 bootsector...\n");
    vfs_iread(dev, 0, sizeof(*bs), (void *)bs);

    char *fstype = (char *)kmalloc(9);
    if(!fstype)
        return -ENOMEM;

    strncpy(fstype, bs->filesys, sizeof(bs->filesys));
    //Not a fat12 bootsector
    if(strcmp(fstype,  "FAT12   "))
        return -EINVAL;

    kfree((void *)fstype);

    fat12_media_desc_t media;
    media.raw = (uint8_t)bs->media;
}

int fat12_mount(const char *dir, int flags, void *data)
{

}

size_t fat12_iread(inode_t *inode, off_t offset, size_t size, void *buf)
{

}

size_t fat12_iwrite(inode_t *inode, off_t offset, size_t size, void *buf)
{

}

int fat12_iclose(inode_t *inode)
{

}