#include <stdio.h>
#include <string.h>
#include <system.h>
#include <fs\vfs.h>
#include <ds\hash.h>
#include <dev\dev.h>
#include <sys\usr.h>
#include <sys\errno.h>
#include <sys\fcntl.h>
#include <sys\panic.h>
#include <mm\kmalloc.h>
#include <dev\console\console.h>

#define VFS_HASHMAX 13

fs_list_t   *registered_fs = NULL;

typedef struct _vfs_hash vfs_hash_t;
static struct _vfs_hash
{
    char *name; //full pathname
    inode_t *node;
    hash_node_t *children; // table of children if isdir
}vfs_root = {
    .name = "/"
};

extern int sysfile_io();

void vfs_init()
{
    kprintfOKmsg("initializing vfs...\n");
    vfs_root.children = mkhash_table(VFS_HASHMAX);
    inode_t inode;
    
    int err;
    if((err = sysfile_io()))
        panic("little or no memory is available to initialize vfs\n");
}

vfs_hash_t fin, bin;
void addnam()
{
    fin.name = "fin";
    fin.children = mkhash_table(VFS_HASHMAX);
    int key = mkhashkey_str(fin.name);
    hash_insert(vfs_root.children, VFS_HASHMAX, key, (void *)&fin);

    bin.name = "bin";
    key = mkhashkey_str(bin.name);
    hash_insert(fin.children, VFS_HASHMAX, key, (void *)&bin);
}

static char **canonicalize_path(const char *const path)
{
    /* Tokenize slash seperated words in path into tokens */
    char **tokens = tokenize(path, '/');
    return tokens;
}

/* Filesystem operations */

void vfs_registerfs(fs_t *fs)
{
    if(!fs)
        panic("vfs_registerfs: failled to register file system");
    fs_list_t *next;
    if (!(next = (fs_list_t *)kmalloc(sizeof(fs_list_t))))
        panic("vfs_registerfs: out of memory");
    memset((void *)next, 0, sizeof(fs_list_t));
    kprintfOKmsg("registering filesystem [%s]...\n", fs->name);
    next->next = registered_fs;
    next->fs = fs;
    next->ready = 0;
    registered_fs = next;
}

int vfs_mount_root(inode_t *root)
{
    if (!root)
        return -EINVAL;
    kprintfOKmsg("mounting vfs root @[%s]...\n", root->name);
    /* TODO Flush mountpoints */
    vfs_root.node = root;
    memset((void *)vfs_root.children, 0, sizeof(hash_node_t) * VFS_HASHMAX);
    return 0;
}

int vfs_mount(const char *type __unused, const char *dir __unused, int flags __unused, inode_t *inode __unused)
{
    return -EINVAL;
}

int vfs_unmount(const char *type __unused, const char *dir __unused, inode_t *inode __unused)
{
    return -EINVAL;
}

//!Path resolution and lookup

int vfs_parse_path(const char *path, const char **abs_path)
{
    if (!path || !*path)
    return -ENOENT;

    char *cwd = cur_usr->cwd;
    if ((*path == '/'))
    cwd = "/";

    size_t cwd_len = strlen(cwd), path_len = strlen(path);
    char *buf = kmalloc(cwd_len + path_len + 2);

    if (strcmp(cwd, "/"))
        memcpy(buf, cwd, cwd_len);
    else
        cwd_len =0;

    buf[cwd_len] = '/';
    memcpy(buf + cwd_len + 1, path, path_len);
    buf[cwd_len + path_len + 1] = 0;

    /* Tokenize slash seperated words in path into tokens */
    char **tokens = canonicalize_path(buf);
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
            valid_tokens[i++] = token;
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

static char *vfs_builtpath(char **tokens)
{
    int len=0, pos =1, count=0, tmp;
    char *path;

    foreach(token, tokens)
    count++;
    if(count> 1)
    --count;
    tmp = count;
    foreach(token, tokens){
        if(!count)
            break;
        len += strlen(token);
        --count;
    }
    
    count = tmp;

    len += (count);
    path = (char *)kmalloc(len);
    *path ='/';
    foreach(token, tokens){
        if(!count)
            break;
        memcpy(path + pos, token, strlen(token));
        pos += strlen(token);
        path[pos++] = '/';
        --count;
    }
    path[len] ='\0';
    return path;
}


static struct vfs_path *vfs_get_mountpoint(char **tokens)
{
    struct vfs_path *path = kmalloc(sizeof(struct vfs_path));
    path->tokens = tokens;
    kprintfOKmsg("getting mountpoint...\n");

    vfs_hash_t *cur_node = &vfs_root;
    vfs_hash_t *last_target_node = cur_node;
    vfs_hash_t *next_vfs_node = NULL;
    size_t token_i = 0;
    int check_last_node = 0;
    
    foreach (token, tokens)
    {
        check_last_node = 0;

        if (cur_node->node)
        {
            last_target_node = cur_node;
            path->tokens = tokens + token_i;
        }

        if (cur_node->children)
        {
            int key = mkhashkey_str(token);

            hash_node_t *tmphash = hash_search(cur_node->children, VFS_HASHMAX, key);
            if (tmphash)
            {
                next_vfs_node = (vfs_hash_t *)tmphash->value;
                cur_node = next_vfs_node;
                check_last_node = 1;
                goto next;
            }
            /* Not found, break! */
            printf("\'%s/%s\': not a file or directory\n", cur_node->name, token);
            break;
        }
        else
        {
            kprintfailure("\'%s\': has no children or is not a dir\n", cur_node->name);
            /* No children, break! */
            break;
        }
    next:;
        ++token_i;
    }

    if (check_last_node && cur_node->node)
    {
        last_target_node = cur_node;
        path->tokens = tokens + token_i;
    }

    path->mountpoint = last_target_node->node;

    return path;
}


int vfs_lookup(const char *path, vnode_t *vnode, const char **abs_path)
{
    int ret = 0;
    vfs_path_t *p = NULL;
    char *tokens = NULL;
    
    /* if path is NULL pointer, or path is empty string, return ENOENT */
    if (!path || !*path)
    return -ENOENT;
    
    kprintfOKmsg("looking up %s\n", path);
    char *_path = NULL;
    if ((ret = vfs_parse_path(path, &_path))){
        kprintfailure("failed to parse path\n");
        goto error;
    }


    kprintfOKmsg("parsed path \'%s\'\n", _path);
    /* Canonicalize Path */
    tokens = canonicalize_path(_path);

    /* Get mountpoint & path */
    p = vfs_get_mountpoint(tokens);

    foreach(token, p->tokens)
        printf("%s ", token);

    printf("\n");
    struct vnode dir, child;

    dir.super = p->mountpoint;
    dir.name = p->mountpoint->name;
    dir.id = p->mountpoint->id;
    dir.type = p->mountpoint->type;
    child.super = p->mountpoint;
    

    foreach (token, p->tokens)
    {
        if ((ret = vfs_vfind(&dir, token, &child))){
            printf("goto error\n");
            goto error;
        }
        memcpy(&dir, &child, sizeof(dir));
    }

    free_tokens(tokens);
    kfree(p);
    memcpy(vnode, &dir, sizeof(dir));

    printf("seting _path = %s\n", _path);
    if (abs_path)
        *abs_path = strdup(_path);

    kfree(_path);

#if 0
    /* Resolve symbolic links */
    if (dir.type == FS_SYMLINK && !(cur_usr->flags & O_NOFOLLOW))
    {
        /* TODO enforce limit */
        char sym[1024];
        struct inode *inode;
        vfs_vget(&dir, &inode);
        vfs_read(inode, 0, 1024, sym);
        int ret = vfs_lookup(sym, uio, vnode, NULL);
        vfs_close(inode);
        return ret;
    }
#endif

    return 0;

error:
    if (tokens)
        free_tokens(tokens);
    if (p)
        kfree(p);
    if (_path)
        kfree(_path);

    return ret;
}

int vfs_vget(struct vnode *vnode, struct inode **inode)
{
    if (!vnode || !vnode->super || !vnode->super->fs)
        return -EINVAL;

    if (!vnode->super->fs->iops.vget)
        return -ENOSYS;

    int ret = vnode->super->fs->iops.vget(vnode, inode);

    if (!ret && inode && *inode)
    {
        (*inode)->ref++;
    }

    return ret;
}

int vfs_vfind(struct vnode *parent, const char *name, struct vnode *child)
{
    if (!parent || !parent->super || !parent->super->fs)
        return -EINVAL;
    if (!parent->super->fs->iops.vfind)
        return -ENOSYS;
    if (parent->type != FS_DIR)
        return -ENOTDIR;

    return parent->super->fs->iops.vfind(parent, name, child);
}

//!file operations

int vfs_fopen(file_t *file)
{
    if ((!file) || (!file->inode))
        return -EINVAL;
    if (ISDEV(file->inode))
        return kdev_open(&_INODE_DEV(file->inode));
    if (!file->inode->fs)
        return -ENOSYS;
    if (!file->inode->fs->fops.open)
        return -EINVAL;
    return file->inode->fs->fops.open(file);
}

size_t vfs_fread(file_t *file, void *buf, size_t size)
{
    if ((!file) || (!file->inode))
        return -EINVAL;
    /*is dev*/
    if (ISDEV(file->inode))
        return kdev_read(&_INODE_DEV(file->inode), file->offset, size, buf);
    /*if other file*/
    if (!file->inode->fs)
        return -ENOSYS;
    if (!file->inode->fs->fops.read)
        return -EINVAL;
    return file->inode->fs->fops.read(file, buf, size);
}

size_t vfs_fwrite(file_t *file, void *buf, size_t size)
{
    if ((!file) || (!file->inode))
        return -EINVAL;
    if (ISDEV(file->inode))
        return kdev_write(&_INODE_DEV(file->inode),file->offset, size, buf);
    if (!file->inode->fs)
        return -ENOSYS;
    if (!file->inode->fs->fops.write)
        return -EINVAL;
    return file->inode->fs->fops.write(file, buf, size);
}

int vfs_fclose(file_t *file)
{
    if ((!file) || (!file->inode))
        return -EINVAL;
    if (ISDEV(file->inode))
        return kdev_close(&_INODE_DEV(file->inode));
    if ((!file->inode->fs))
        return -ENOSYS;
    if ((!file->inode->fs->fops.close))
        return -EINVAL;
    return file->inode->fs->fops.close(file);
}


//!inode operations mappings

size_t vfs_iread(inode_t *inode, off_t offset, size_t size, void *buf)
{
    if((!inode) || (!size) || (!buf))
        return -EINVAL;
    if(ISDEV(inode))
        return kdev_read(&_INODE_DEV(inode), offset, size, buf);
    if(!inode->fs)
        return -ENOSYS;
    if(!inode->fs->iops.iread)
        return -EINVAL;
    return inode->fs->iops.iread(inode, offset, size, buf);
}

size_t vfs_iwrite(inode_t *inode, off_t offset, size_t size, void *buf)
{
    if((!inode) || (!size) || (!buf))
        return -EINVAL;
    if(ISDEV(inode))
        return kdev_write(&_INODE_DEV(inode), offset, size, buf);
    if(!inode->fs)
        return -ENOSYS;
    if(!inode->fs->iops.iwrite)
        return -EINVAL;
    return inode->fs->iops.iwrite(inode, offset, size, buf);
}

int vfs_iclose(inode_t *inode)
{
    if(!inode)
        return -EINVAL;
    if(ISDEV(inode))
        return kdev_close(&_INODE_DEV(inode));
    if(!inode->fs)
        return -ENOSYS;
    if(!inode->fs->iops.iclose)
        return -EINVAL;
    return inode->fs->iops.iclose(inode);
}

/* Higher level functions */

int vfs_creat(const char *path __unused, inode_t **ref __unused)
{
    return -EINVAL;
}