#include <string.h>
#include <system.h>
#include <fs\vfs.h>
#include <fs\vfs.h>
#include <dev\dev.h>
#include <ds\queue.h>
#include <sys\errno.h>
#include <sys\panic.h>
#include <sys\cdefs.h>
#include <boot\boot.h>
#include <mm\kmalloc.h>



struct inode *ramdisk_dev_node = NULL;
static queue_t *archivers = QUEUE_NEW();

void initramfs_archiver_register(struct fs *fs)
{
    enqueue(archivers, fs);
}

void load_ramdisk()
{
    kprintfOKmsg("loading ramdisk...\n");

    ramdisk_dev_node = kmalloc(sizeof(struct inode));
    memset(ramdisk_dev_node, 0, sizeof(struct inode));

    extern size_t ramdev_sz;

    *ramdisk_dev_node = (struct inode){
        .name = "ramdev",

        .type = FS_BLKDEV,
        .rdev = _DEV_T(0, 0),

        .size = ramdev_sz,
    };

    struct inode *root = NULL;
    int err = -1;
    root = (inode_t *)kmalloc(sizeof(inode_t));
    if(!root){
        kprintfailure("failed to load ramdisk!\n");
        return;
    }
    
    forlinked(node, archivers->head, node->next)
    {
        struct fs *fs = node->value;
        if (!(err = fs->load(ramdisk_dev_node, &root)))
            break;
    }

    if (err)
        panic("err_code[%p]: could not load ramdisk\n", err);

    vfs_mount_root(root);
}