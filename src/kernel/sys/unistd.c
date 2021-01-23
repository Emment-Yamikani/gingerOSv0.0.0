#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <system.h>
#include <fs\vfs.h>
#include <sys\cdefs.h>
#include <sys\errno.h>
#include <sys\fcntl.h>
#include <mm\kmalloc.h>

#define SYSFDSMAX 128

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
        if(!sys_fds[i].inode)
        {
            sys_fds[i].inode = (inode_t *)(-1);
            return i;
        }
    }
    return -ENOFDS;
}

void sys_releasefd(int fd)
{
    if ((fd >= SYSFDSMAX) || !sys_fds)
        return;
    sys_fds[fd].inode =0;
    sys_fds[fd].offset =0;
    sys_fds[fd].flags =0;
}

int open(const char *path, int mode)
{
    int fd = sys_getfd();
    if( fd == -ENOFDS)
    return -ENOFDS;
    
    vnode_t vnode;
    inode_t *inode = NULL;
    char * abs_path;
    kprintfOKmsg("opening file %s\n", path);
    int ret = vfs_lookup(path, &vnode, &abs_path);
    printf("abspath=%s\n", abs_path);
    if(ret){//not found, so create
        kprintfailure("not found, creating it...\n");
        if((ret == -ENOENT) && (mode & O_CREAT))
        {
            if((ret = vfs_creat(path, &inode))){
                kprintfailure("failed to creat %s\n", path);
                goto error;
            }
            goto o_creat;
        }
    }

    ret = vfs_vget(&vnode, &inode);

    if(ret)
        goto error;

o_creat:
    kprintfOKmsg("O_CREATing\n");
    sys_fds[fd] = (file_t){
        .flags = mode,
        .inode = inode,
        .offset =0
    };

    ret = vfs_fopen(&sys_fds[fd]);

error:
    kprintfailure("error occured\n");
    if(ret < 0){
    sys_releasefd(fd);
        if(inode)
            kfree((void *)inode);
        return -EINVAL;
    }
    else ret = fd;
    return ret;
}

int creat(const char *path, int mode)
{
    return open(path, mode);
}

size_t read(int fd, void *buf, size_t size)
{
    if(fd >= SYSFDSMAX)
        return -EBADFD;
    file_t *file = &sys_fds[fd];
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