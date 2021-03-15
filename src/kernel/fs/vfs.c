#include <stdio.h>
#include <string.h>
#include <system.h>
#include <fs/fs.h>
#include <dev/dev.h>
#include <sys/usr.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/panic.h>
#include <mm/kmalloc.h>
#include <dev/console/console.h>


struct fs_list *registered_fs;
/*virtual filesystem root*/
struct dentry *vroot;

extern int sysfile_io();


void vfs_init()
{
    kprintfOKmsg("initializing vfs...\n");
    if(sysfile_io())
        panic("little or no memory is available to initialize vfs\n");
}

int vfs_registerfs(fs_t *fs)
{
    if (!fs)
        panic("vfs_registerfs: failled to register file system");
    fs_list_t *next;
    if (!(next = (fs_list_t *)kmalloc(sizeof(fs_list_t))))
        panic("vfs_registerfs: out of memory!");
    memset((void *)next, 0, sizeof(fs_list_t));
    next->next = registered_fs;
    next->type = fs;
    registered_fs = next;
    kprintfOKmsg("vfs_registerfs: registered filesystem [%s]\n", fs->fs_name);
    return 0;
}

int vfs_mount_root(dentry_t *root)
{
    int err =0;
    kprintfOKmsg("flushing vroot...\n");

    if(!root){
        kprintfailure("can't flush vroot: couldn't find root");
        return -EINVAL;
    }

    vroot =  root;

    if(!vroot->dchildren)
    {
        vroot->dchildren = mkhash_table(DCACHE_COUNT);
        if (!vroot->dchildren){
            err = -ENOMEM;
            goto error;
        }
    }

    memset((void *)vroot->dchildren, 0, sizeof(*vroot->dchildren));
    vroot->populated =0;
    vroot->sync_pending =1;

    /*if(err = vfs_populate_dir(vroot)){
        kprintfailure("couldn't flush vroot\n");
        goto error;
    }*/

    return 0;

error:
    if(vroot){
        if(vroot->dchildren)
            kfree(vroot->dchildren);
        kfree(vroot);
    }

    return err;
}

int vfs_populate_dir(dentry_t *dentry)
{
    int err =0;
    if(!dentry){ err = -EINVAL; goto error; }

    //vfs_iread(dentry->dnode, 0, dentry->dnode->isize, (void *)dentry->dchildren);
    return 0;
error:
    return err;
}

/*         *****            */
/*      VFS HELPERS         */
/*         *****            */

struct path *vfs_get_mountpoint();

int vfs_find(dentry_t *parent, const char *name, dentry_t **child)
{
    int err =0;
    if(!parent || !parent->dnode)
    { kprintfailure("vfind: !dnode\n"); err = -EINVAL; goto error;}
    if(!parent->dnode->isb || !parent->dnode->isb->sfstype || !parent->dnode->isb->sfstype->iops.ifind)
    { kprintfailure("vfind: !ifind\n"); err = -ENOSYS; goto error;}

    /*do name-syntax check here*/
    if(!name){ err = -ENOTNAM; goto error;}

    if(parent->dnode->itype != DIR) { err = -ENOTDIR; goto error;}
    if(!parent->dchildren) { err = -EINVAL; goto error;}
    /*do cache traversal*/
    int hkey = mkhashkey_str(name);
    dentry_t *dentry = (dentry_t *)hash_search(parent->dchildren, DCACHE_COUNT, hkey);

    if(!dentry){
        kprintfailure("vfind: %s not in cache\n", name);
        parent->cache_misses++;
        /*if cache misses > 8, cache it*/
        if(err = parent->dnode->isb->sfstype->iops.ifind(parent, name, &dentry)){ goto error;}
        hash_insert(parent->dchildren, DCACHE_COUNT, hkey, (void *)dentry);
    }
    else kprintfOKmsg("vfind: %s present in cache\n", name);

    if(child)
        *child = dentry;
    
    return 0;

error:
    return err;
}

int vfs_parse_path(const char *path, const char *cwd, const char **abs_path)
{

    kprintfOKmsg("parsing path:%s\n", path);
    if (!path || !*path)
        return -ENOENT;

    if (*path == '/')
    { /* Absolute path */
        cwd = "/";
    }
    

    size_t cwd_len = strlen(cwd), path_len = strlen(path);
    char *buf = kmalloc(cwd_len + path_len + 2);

    memcpy(buf, cwd, cwd_len);


    buf[cwd_len] = '/';
    memcpy(buf + cwd_len + 1, path, path_len);
    buf[cwd_len + path_len + 1] = 0;

    /* Tokenize slash seperated words in path into tokens */
    char **tokens = tokenize(buf, '/');
    char *out = kmalloc(cwd_len + path_len + 1);

    char *valid_tokens[512];
    size_t i = 0;

    foreach (token, tokens)
    {
        if (token[0] == '.')
        {
            if (token[1] == '.' && i > 0)
                valid_tokens[--i] = NULL;
        }
        else
        {
            valid_tokens[i++] = token;
        }
    }

    valid_tokens[i] = NULL;

    out[0] = '/';

    size_t j = 1;
    foreach (token, valid_tokens)
    {
        size_t len = strlen(token);
        memcpy(out + j, token, len);
        j += len;
        out[j] = '/';
        ++j;
    }

    out[j > 1 ? --j : 1] = 0;

    free_tokens(tokens);
    kfree(buf);

    if (abs_path)
        *abs_path = out;
    else
        kfree(out);

    return 0;
}

int vfs_lookup(const char *path, dentry_t **dentry)
{
    int err =0;
    if(!path)
    {
        kprintfailure("vfs_lookup: \"%s\" is not a file or directory\n");
        err = -EINVAL;
        goto error;
    }

    char *abs_path = NULL;
    if(err = vfs_parse_path(path, "/", &abs_path)) { goto error;}
    char **tokens = canonicalize_path(abs_path);
    dentry_t *parent, *child = NULL, *cur = NULL;

    cur = parent = vroot;
    foreach(token, tokens)
    {
        if((err = vfs_find(parent, token, &child)))
            goto error;
        if(!err)
            goto found;
    }

    found:
    free_tokens(tokens);
    if(dentry)
        *dentry = child;
    return 0;
error:
    if (tokens)
        free_tokens(tokens);
    return err;
}

#define ISDEV(inode) ((inode)->itype == CHRDEV || (inode)->itype == BLKDEV)

size_t vfs_iread(struct inode *inode, size_t offset, size_t size, void *buf)
{
    /* Invalid request */
    if (!inode)
        return -EINVAL;

    /* Device node */
    if (ISDEV(inode))
        return kdev_read(&_INODE_DEV(inode), offset, size, buf);

    /* Invalid request */
    if (!inode->isb->sfstype)
        return -EINVAL;

    /* Operation not supported */
    if (!inode->isb->sfstype->iops.iread)
        return -ENOSYS;

    return inode->isb->sfstype->iops.iread(inode, offset, size, buf);
}

size_t vfs_iwrite(struct inode *inode, size_t offset, size_t size, void *buf)
{
    /* Invalid request */
    if (!inode)
        return -EINVAL;

    /* Device node */
    if (ISDEV(inode))
        return kdev_write(&_INODE_DEV(inode), offset, size, buf);

    /* Invalid request */
    if (!inode->isb->sfstype)
        return -EINVAL;

    /* Operation not supported */
    if (!inode->isb->sfstype->iops.iwrite)
        return -ENOSYS;

    return inode->isb->sfstype->iops.iwrite(inode, offset, size, buf);
}

int vfs_iclose(struct inode *inode)
{
    /* Invalid request */
    if (!inode || !inode->isb->sfstype)
        return -EINVAL;

    /* Operation not supported */
    if (!inode->isb->sfstype->iops.iclose)
        return -ENOSYS;

    --inode->iref;

    if (inode->iref <= 0)
    { /* Why < ? */
        return inode->isb->sfstype->iops.iclose(inode);
    }

    return 0;
}

int vfs_fopen(struct file *file)
{
    if (!file || !file->fnode || !file->fnode->isb || !file->fnode->isb->sfstype)
        return -EINVAL;

    if (file->fnode->itype == DIR && !(file->fflags & O_SEARCH))
        return -EISDIR;

    if (ISDEV(file->fnode))
        return kdev_file_open(&_INODE_DEV(file->fnode), file);

    if (!file->fnode->isb)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fopen)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.fopen(file);
}

size_t vfs_fread(file_t *file, void *buf, size_t size)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_read(&_INODE_DEV(file->fnode), file, buf, size);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fread)
        return -ENOSYS;
    return file->fnode->isb->sfstype->fops.fread(file, buf, size);
}

size_t vfs_fwrite(struct file *file, void *buf, size_t nbytes)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_write(&_INODE_DEV(file->fnode), file, buf, nbytes);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fwrite)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.fwrite(file, buf, nbytes);
}

off_t vfs_flseek(struct file *file, off_t offset, int whence)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_lseek(&_INODE_DEV(file->fnode), file, offset, whence);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.flseek)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.flseek(file, offset, whence);
}

int vfs_fclose(struct file *file)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_close(&_INODE_DEV(file->fnode), file);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fclose)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.fclose(file);
}

int vfs_fcan_read(struct file *file, size_t size)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_can_read(&_INODE_DEV(file->fnode), file, size);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fcan_read)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.fcan_read(file, size);
}

int vfs_fcan_write(struct file *file, size_t size)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_can_write(&_INODE_DEV(file->fnode), file, size);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    if (!file->fnode->isb->sfstype->fops.fcan_write)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.fcan_write(file, size);
}

int vfs_feof(struct file *file)
{
    if (!file || !file->fnode)
        return -EINVAL;

    if (ISDEV(file->fnode))
        return kdev_file_eof(&_INODE_DEV(file->fnode), file);

    if (!file->fnode->isb)
        return -EINVAL;

    if (!file->fnode->isb->sfstype)
        return -ENOSYS;

    return file->fnode->isb->sfstype->fops.eof(file);
}