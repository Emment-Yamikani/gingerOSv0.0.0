#ifndef INODE_H
# define INODE_H
#include <stdint.h>
#include <stddef.h>
#include <sys\cdefs.h>

struct fs;
typedef struct fs fs_t;

typedef enum
{
    _INVALID,
    FS_RGL,
    FS_DIR,
    FS_CHRDEV,
    FS_BLKDEV,
    FS_SYMLINK,
    FS_PIPE,
    FS_FIFO,
    FS_SOCKET,
    FS_SPECIAL,
} file_type_t;

/*
!only one copy is allowed*/
typedef struct inode
{
    //identification and enumaration
    char *name;       //name of file
    uintptr_t id;     //(unique id)
    size_t size;      //sizeof(file)
    file_type_t type; //type of file

    int rdev;

    //*Attributes
    int flags;  //flags
    int uid;    //user ID
    int gid;    //group ID
    int mask;   //file access mask
    int nlinks; //#of links to this inode
    int ref;    //refrence count

    void *p; //filesystem private data
    fs_t *fs;   //belongs to which filesystem?
} inode_t;

/*multiple copies allowed*/
typedef struct vnode
{
    inode_t *super;
    char *name;
    int mask;
    uintptr_t id;
    uid_t uid;
    gid_t gid;
    file_type_t type;
}vnode_t;


#endif