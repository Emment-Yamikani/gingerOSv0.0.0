#include <ctype.h>
#include <sizes.h>
#include <stdio.h>
#include <string.h>
#include <system.h>
#include <dev/dev.h>
#include <sys/usr.h>
#include <fs/fat12.h>
#include <fs/posix.h>
#include <fs/ramfs.h>
#include <ds/queue.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/errno.h>
#include <sys/panic.h>
#include <boot/boot.h>
#include <mm/kmalloc.h>
#include <core/module.h>

fs_t ramfs;
fat12bs_t *ramfs_bs = NULL;
super_block_t ramfs_sb;
struct dentry *droot;
ramfs_inode_t ramfs_root;
queue_t ramfs_icache;

inode_t *Rdev;

static struct _ramfs_desc
{
    fat12_dirent_t *fat12rootdir;
    size_t fatsz; /*size of each fat*/
    size_t fat0_off;
    size_t fat1_off;
    uint16_t *fat0_data;
    uint16_t *fat1_data;
    size_t numfatsectors;

    size_t data_off;
    size_t numdatasectors;
    
    size_t rootsz; /*size of rootdir*/
    size_t blocksz;
    size_t root_off;
    size_t numrootsectors;
    size_t numrootentries;
    size_t dirent_per_block;
}ramfs_desc;

int ramfs_init()
{
    int err=0;
    initramfs_archiver_register(&ramfs);
    droot = (dentry_t *)kmalloc(sizeof(dentry_t));
    if (!droot){
        err = -ENOMEM;
        kprintfailure("ramfs_init: no memory to alloc ramfs-droot\n");
        goto error;
    }
    
    memset((void *)&ramfs_icache, 0, sizeof(ramfs_icache));
    ramfs_sb.sroot = droot;
    droot->dchildren = mkhash_table(DCACHE_COUNT);
    if (!droot->dchildren)
    {
        err = -ENOMEM;
        goto error;
    }
    droot->dname = "/";

    memset((void *)&ramfs_desc, 0, sizeof(ramfs_desc));
    if( err = vfs_registerfs(&ramfs))
        goto error;
    return 0;
error:
    if (droot){
        if(droot->dchildren)
            kfree((void *)droot->dchildren);
        kfree((void *)droot);
    }
    return err;
}

int ramfs_root_inode(inode_t *dev, inode_t **inode)
{
    int err = 0;
    struct inode *node = NULL;

    if (!(node = kmalloc(sizeof(struct inode))))
    {
        err = -ENOMEM;
        goto error;
    }

    Rdev = dev;
    node->iino = node;
    node->isize = 0;
    node->itype = DIR;
    node->isb = &ramfs_sb;
    node->isb->sfstype = &ramfs;
    node->iprivate = 0;

    node->iuid = 0;
    node->igid = 0;
    node->imode = 0555; /* r-xr-xr-x */
    node->inlinks = 2;

    if (inode)
        *inode = node;

    return 0;

error:
    if (node)
        kfree(node);

    return err;
}

int ramfs_load(inode_t *dev, dentry_t **root)
{
    int err = 0;
    inode_t *ramfs_iroot = NULL;
    size_t size  = 0;
    off_t offset = 0;

    ramfs_bs = (fat12bs_t *)kmalloc(sizeof(*ramfs_bs));
    if(!ramfs_bs)
    {
        err = -ENOMEM;
        goto error;
    }

    memset((void *)ramfs_bs, 0, sizeof(*ramfs_bs));

    kprintfOKmsg("ramfs_load: searching for bootloader...\n");
    size = 512; /*fat12 bootsector is 512B*/
    vfs_iread(dev, offset, size, (void *)ramfs_bs);

    /*check bootload validity*/
    char fsname[9] = {0};
    memcpy(fsname, ramfs_bs->filesys, 8);
    fsname[8] =0;

    if(strcmp(fsname, "FAT12   "))
    {
        kprintfailure("ramfs_load: not a FAT12 bootloader\n");
        err = -ENOENT;
        goto error;
    }
    if(ramfs_bs->boot_magic != 0xaa55)
    {
        kprintfailure("ramfs_load: not a FAT12 bootloader\n");
        err = -ENOENT;
        goto error;
    }

    kprintfOKmsg("ramfs_load: bootloader found\n");
    extern size_t ramdev_sz;
    ramfs_sb.sblocksize = ramfs_bs->bytes_per_sect * ramfs_bs->sects_per_cluster;
    ramfs_sb.scount = 0;
    ramfs_sb.sdev = _DEV_T(0, 0);
    ramfs_sb.sflags = ramfs_bs->media;
    ramfs_sb.sfsinfo = &ramfs_desc;
    ramfs_sb.smagic = ramfs_bs->ext_boot_sign;
    ramfs_sb.smaxbytes = _1M + _4K*100 + 40;
    ramfs_sb.sroot = droot;
    ramfs_sb.ssize = ramdev_sz;

    kprintfOKmsg("preparing to mount root...\n");
    ramfs_desc.blocksz = ramfs_sb.sblocksize;
    
    ramfs_desc.fat0_off = ramfs_bs->reserved_sects * ramfs_desc.blocksz;
    ramfs_desc.numfatsectors = ramfs_bs->no_fats * ramfs_bs->sector_per_fat;

    ramfs_desc.numrootentries = ramfs_bs->max_root_entries;
    ramfs_desc.dirent_per_block = ramfs_sb.sblocksize / sizeof(fat12_dirent_t);
    
    //!preparando' fat12 filesystem
    
    ramfs_desc.numrootsectors = (ramfs_desc.numrootentries * sizeof(fat12_dirent_t)) / ramfs_desc.blocksz;
    ramfs_desc.rootsz = ramfs_desc.numrootsectors * ramfs_bs->bytes_per_sect;
    ramfs_desc.root_off = (ramfs_bs->reserved_sects + ramfs_desc.numfatsectors) * ramfs_bs->bytes_per_sect;
    
    ramfs_desc.data_off = ramfs_bs->reserved_sects + ramfs_desc.numfatsectors + ramfs_desc.numrootsectors;
    
    ramfs_desc.numdatasectors =
    (ramfs_bs->total_sects 
    - (ramfs_bs->reserved_sects + ramfs_desc.numfatsectors + ramfs_desc.numrootsectors));
    //*read primary FAT
    size = (ramfs_bs->sector_per_fat * ramfs_sb.sblocksize);
    ramfs_desc.fat0_data = (uint16_t *)kmalloc(size);
    if (ramfs_desc.fat0_data) memset((void *)ramfs_desc.fat0_data, 0, size);
    else
    {
        kprintfailure("ramfs_load: failed to load primary FAT\n");
        err = -ENOMEM;
        goto error;
    }
    
    kprintfOKmsg("reading primary FAT...\n");
    offset = ramfs_desc.fat0_off;
    vfs_iread(dev, offset, size, (void *)ramfs_desc.fat0_data);

    
    size = ramfs_desc.rootsz;
    offset = ramfs_desc.root_off;

    ramfs_desc.fat12rootdir = (fat12_dirent_t *)kmalloc(size);
    if (ramfs_desc.fat12rootdir)
        memset((void *)ramfs_desc.fat12rootdir, 0, size);
    else
    {
        kprintfailure("ramfs_load: failed to load rootdir\n");
        err = -ENOMEM;
        goto error;
    }

    kprintfOKmsg("reading rootdir...\n");
    vfs_iread(dev, offset, size, (void *)ramfs_desc.fat12rootdir);

    ramfs_root_inode(dev, &ramfs_iroot);

    if(!droot)
    {

        if(!(droot = (dentry_t *)kmalloc(sizeof(dentry_t)))){
            err = -ENOMEM;
            goto error;
        }

        memset((void *)droot, 0, sizeof(*droot));
        ramfs_sb.sroot = droot;
        droot->dchildren = mkhash_table(DCACHE_COUNT);

        if(!droot->dchildren)
        {
            err = -ENOMEM;
            goto error;
        }

        droot->dname = "/";
        droot->cache_misses =0;
        droot->sync_pending =1;
        droot->refs         =0;
        droot->populated    =0;
        droot->dparent      =NULL;
        droot->dcount       =0;
        droot->cached       =0;

    }

    droot->dnode = ramfs_iroot;
    droot->dnode->iprivate = (void *)&ramfs_root;

    struct queue_node *node = enqueue(&ramfs_icache, (void *)&ramfs_root);
    if(!node){ err = -ENOMEM; goto error;}

    droot->dnode->iino = node;
    ramfs_root.name = strdup("/");
    ramfs_root.cluster0 = ramfs_desc.root_off / ramfs_bs->bytes_per_sect;
    ramfs_root.inmem_metadata = 0;
    ramfs_root.parent_cluster0 = 0;
    ramfs_root.size = 0;
    ramfs_root.type = FAT_DIR;

    if (root)
        *root = droot;

    /*return 0 upon success*/
    kprintfOKmsg("ramfs_load: bootsector loaded successfully\n");
    return 0;

    /**
     * print err msg &
     * return 'err' on failure*/
error:
    kprintfailure("ramfs_load: failed to load ramfs\n"
                  "releasing resources...\n");
    if(droot){
        kfree((void *)droot->dchildren);
        kfree(droot);
    }
    if(ramfs_desc.fat0_data)
        kfree(ramfs_desc.fat0_data);
    if(ramfs_desc.fat1_data)
        kfree(ramfs_desc.fat1_data);
    if(ramfs_desc.fat12rootdir)
        kfree(ramfs_desc.fat12rootdir);
    if(ramfs_bs)
        kfree(ramfs_bs);
    return err;
}

int ramfs_mount()
{
    kprintfailure("ramfs_mount: ramfs_mount not ready...\n");
    return -EINVAL;
}

int ramfs_kill_sb(super_block_t *sb __unused)
{
    kprintfailure("ramfs_killsb: kill super block not ready...\n");
    return-EINVAL;
}

ramfs_inode_t *ramfs_inode_convert(inode_t *inode)
{
    if (!inode)
        return NULL;
    return (ramfs_inode_t *)inode->iprivate;
}
/*
char *ramfs_parse_filename(const char *fname)
{
    if(!fname)
        return NULL;
    if(strlen(fname) > 12)
        return NULL;

    char *dosname = (char *)kmalloc(12);
    char **tokens = tokenize(fname, '.');

    int token_count =0;
    int len =0;
    int pos =0;
    int space_padding =0;
    int prev_len=0;


    foreach(token, tokens)
        len += strlen(token);
    
    space_padding = 11 - len;

    foreach(token, tokens)
    {
        token_count ++;
        if(token_count > 2)
        {
            kprintfailure("invalid filename [%s]\n", fname);
            return -EINVAL;
        }
        prev_len = strlen(token);
        char *tmpstr = toupperstr(token);
        memcpy(dosname + pos, tmpstr, prev_len);
        kfree((void*)tmpstr);
        pos += prev_len;
        if(token_count < 2)
            memset((void *)dosname +pos, ' ', space_padding);
        pos +=space_padding;
    }

    return dosname;

}
*/

char ramfsIObuf[512];

size_t ramfs_iread(inode_t *inode, off_t offset, size_t size, void *buf)
{
    ramfs_inode_t *node = ramfs_inode_convert(inode);
    int cluster = size / ramfs_desc.blocksz;
    if(size % ramfs_desc.blocksz)
        cluster++;

    size_t cpysz =0;
    size_t rw_offset =0;

    for (;cluster; --cluster){
        uint16_t fatoff = (node->current_cluster + (node->current_cluster /2));

        uint16_t pos = (31 + node->current_cluster) * ramfs_desc.blocksz;
        vfs_iread(Rdev, pos, ramfs_desc.blocksz, ramfsIObuf);

        if(size < ramfs_desc.blocksz){
            memcpy(buf + rw_offset, ramfsIObuf, size);
            rw_offset +=  size;
            cpysz += size;
            goto done;
        }
        else{
            memcpy(buf + rw_offset, ramfsIObuf, ramfs_desc.blocksz);
            rw_offset +=  ramfs_desc.blocksz;
            size -= ramfs_desc.blocksz;
        }

        uint16_t fatsector = (fatoff / ramfs_desc.blocksz);
        uint16_t fatentryoff = fatoff % ramfs_desc.blocksz;
        fatoff = fatsector * ramfs_desc.blocksz;

        uint16_t *FAT = ramfs_desc.fat0_data + fatoff;

        uint16_t next_cluster = FAT[fatentryoff];

        if(node->current_cluster && 1)
            next_cluster >>= 4;
        else
            next_cluster &= 0xfff;

        if(next_cluster >= 0xff8)
            return -1;
        if(next_cluster == 0)
            goto failure;
        node->current_cluster = next_cluster;
    }

done:
    node->offset += rw_offset;
    return cpysz;

failure:
    return -EINVAL;
}


int ramfs_ifind(dentry_t *parent, const char *name, dentry_t **child)
{
    kprintfOKmsg("ramfs_ifind: searching for '%s' in '%s'\n", name, parent->dname);
    ramfs_inode_t * node = ramfs_inode_convert(parent->dnode);
    if (!node)
        return -EINVAL;
    int off = node->cluster0 * ramfs_desc.blocksz;
    size_t size =0;
    fat12_dirent_t *dir = ramfs_desc.fat12rootdir;

    char parsed[12];
    if (parseToDos(parsed, name)) kprintfailure("invalid name\n");
    if(strcmp(node->name, "/")){
        kprintfailure("resolve cluster searching issue\n");
        vfs_iread(Rdev, off, size, dir);
    }

    for(int i=0; i < ramfs_desc.numrootentries; ++i)
    {
        if(!strcmp(dir[i].name, parsed)){
            ramfs_inode_t *irfs = (ramfs_inode_t *)kmalloc(sizeof(ramfs_inode_t));
            if(!irfs)
                return -ENOMEM;
            irfs->cluster0 = dir[i].first_cluster;
            irfs->current_cluster = dir[i].first_cluster;
            irfs->inmem_metadata = NULL;
            irfs->name = strdup(name);
            irfs->parent_cluster0 = node->cluster0;
            if((dir[i].attrib && 0x10))
                irfs->type = FAT_DIR;
            else irfs->type = FAT_FILE;
            
            inode_t *inode = (inode_t *)kmalloc(sizeof(*inode));
            if(!inode)
            return -ENOMEM;
            
            memset((void *)inode, 0, sizeof(*inode));
            inode->iino = enqueue(&ramfs_icache, (void *)irfs);
            if(!inode->iino)
                return -ENOMEM;
            
            inode->isize = irfs->size = dir[i].size;
            inode->imode = 0555;
            inode->iprivate = (void *)irfs;
            inode->isb = &ramfs_sb;
            if(irfs->type == FAT_DIR)
                inode->itype = DIR;
            else if(irfs->type == FAT_FILE)
                inode->itype = RGL;
            else inode->itype = FAT_UNDEF;

            dentry_t *dentry = (dentry_t *)kmalloc(sizeof(dentry_t));
            if(!dentry)
                return -ENOMEM;
            memset((void *)dentry, 0, sizeof(dentry_t));
            dentry->dname = strdup(name);
            dentry->dnode = inode;
            dentry->dparent = parent;

            if(child)
                *child = dentry;
            return 0;
        }
    }

    return -ENOENT;
}

fs_t ramfs = {
    .fs_name = "FAT12",
    .fs_load = ramfs_load,
    .fs_mount = ramfs_mount,
    .fops = {
        .fread = posix_file_read,
        .flseek = posix_file_lseek,
        .fclose = posix_file_close,
        .fopen = posix_file_open,
        .fcan_read = posix_file_can_read,
        .fcan_write = posix_file_can_write,
        .eof = posix_file_eof
    },
    .iops = {
        .iread = ramfs_iread,
        .ifind = ramfs_ifind
    },
    .fs_kill_sb = ramfs_kill_sb,
};

super_block_t ramfs_sb = {
    .uuid = 
    {'N','E','R','O','N','O','L','T','-','R','A','M','F','S', 0},
    .sfstype = &ramfs
};

MODULE_INIT(ramfs, ramfs_init, NULL)