#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <system.h>
#include <fs/fs.h>
#include <sys/cdefs.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <mm/kmalloc.h>

#define SYSFDSMAX 64

static file_t *sys_fds = NULL;

int sysfile_io()
{
    sys_fds = kcalloc(SYSFDSMAX, sizeof(file_t));
    if(!sys_fds)
        return -ENOMEM;
    return 0;
}

int sys_getfd()
{
    if(!sys_fds)
        return -EINVAL;
    for(int i=0; i < SYSFDSMAX; ++i)
    {
        if(!sys_fds[i].fnode)
        {
            sys_fds[i].fnode = (inode_t *)(-1);
            return i;
        }
    }
    return -ENOFDS;
}

void sys_releasefd(int fd)
{
    if ((fd >= SYSFDSMAX) || !sys_fds)
        return;
    sys_fds[fd].fnode =0;
    sys_fds[fd].foffset =0;
    sys_fds[fd].fflags =0;
}

int open(const char *path, int mode)
{
    int fd = sys_getfd();
    if( fd == -ENOFDS)
        return -ENOFDS;
    int ret = -EINVAL;

    dentry_t *dentry = NULL;
    if(ret = vfs_lookup(path, &dentry))
        printf("fialed to open file [%s]\n", path);

    sys_fds[fd].fflags = FD_VALID;
    sys_fds[fd].fmode = mode;
    sys_fds[fd].fname = path;
    sys_fds[fd].fnode = dentry->dnode;
    sys_fds[fd].foffset = 0;
    /*sys_fds[fd].fpath->dentry = dentry;
    sys_fds[fd].fpath->mnt->mnt_flags;
    sys_fds[fd].fpath->mnt->mnt_root;
    sys_fds[fd].fpath->mnt->mnt_sb = dentry->dnode->isb;*/
    return fd;
}

int creat(const char *path, int mode)
{
    return open(path, mode);
}

size_t read(int fd, void *buf, size_t size)
{
    if((unsigned int)fd >= SYSFDSMAX)
        return -EBADFD;
    file_t *file = &sys_fds[fd];
    if(!(file->fflags && FD_VALID))
        return -EINVAL;
    return vfs_fread(file, buf, size);
}

size_t write(int fd, void *buf, size_t size)
{
    if(fd >= SYSFDSMAX)
        return -EBADFD;
    file_t *file = &sys_fds[fd];
    return vfs_fwrite(file, buf, size);
}

long lseek(int fd, off_t offset __unused, int whence __unused)
{
    if( fd >= SYSFDSMAX)
        return -EBADFD;
    file_t *file __unused = &sys_fds[fd];
    kprintfailure("lseek: not ready\n");
    return -EINVAL;
}

int close(int fd)
{
    if( fd >= SYSFDSMAX)
        return -EBADFD;
    file_t *file = &sys_fds[fd];
    return vfs_fclose(file);
}