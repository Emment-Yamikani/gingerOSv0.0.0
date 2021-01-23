#ifndef SYS_USR_H
# define SYS_USR_H


typedef struct usr{
    char *name;
    char *rootdir;
    char *cwd;
    char *passwd;
    int uid;
    int gid;
    int mask;
    int sys:1;
}usr_t;

extern usr_t *cur_usr;

extern void usr_init();
extern void usr_start_session();
extern int usr_login();

#endif