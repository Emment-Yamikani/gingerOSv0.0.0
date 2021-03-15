#include <sys\thread.h>
#include <sys\errno.h>


int proc_get_fd(proc_t *p)
{
    for(int i=0; i < MAX_FDS; ++i)
    {
        if(!p->fd[i].fnode)
        {
            p->fd[i].fnode = (inode_t *)(-1);
            return i;
        }
    }
    return -1;
}

int proc_release_fd(proc_t *p, int fd)
{
    if(fd >= MAX_FDS)
        return -EBADF;
    p->fd[fd].fnode = (inode_t *)(NULL);
    return 0;
}