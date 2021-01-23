#include <system.h>
#include <sys\usr.h>
#include <stddef.h>
#include <string.h>
#include <mm\kmalloc.h>
#include <core\module.h>


usr_t *cur_usr = (usr_t *)NULL;

static usr_t root;
static int uid=0;
static usr_t *users = (usr_t *)NULL;

#define MAX_USERS 8


void usr_init()
{
    users = (usr_t *)kcalloc(MAX_USERS, sizeof(usr_t));
    root.name = strdup("root");
    root.passwd = strdup("<?$y554/.");
    root.gid = root.uid = uid;
    root.cwd = strdup("/");
    root.rootdir = strdup(root.cwd);
    root.mask = 0;
    root.sys = 1;//!system usr
    users[uid++] = root;
    cur_usr = &root;
    kprintfOKmsg("starting new session...\n");
}

void usr_start_session()
{

}

int usr_login()
{
    return 1;
}

MODULE_INIT(usr, usr_init, NULL);