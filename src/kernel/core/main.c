#include <ctype.h>
#include <stdio.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <fs\vfs.h>
#include <sys\usr.h>
#include <dev\dev.h>
#include <sys\fcntl.h>
#include <sys\panic.h>
#include <sys\unistd.h>
#include <mm\kmalloc.h>
#include <core\module.h>
#include <fs\ramfs\ramfs.h>
#include <dev\console\console.h>


char buf[512];
extern void addnam();
void main()
{
    modules_init();
    vfs_init();
    load_ramdisk();
    cons_lcyan();
    printf("\n                       Welcome to %sOs-%s\n", OSNAME, OSVERSION);
    cons_cyan();
    printf("                          Liveliness and energy\n");
    cons_reset();

    kprintprompt(cur_usr->name, cur_usr->cwd);

    addnam();
    printf("\n");
    char *path = "fin/bin/osinfo.txt";
    int fd = open(path, O_RDONLY);
    if(fd < 0){
        kprintfailure("ERROR(%X), couldn't open: \'%s\'\n", fd, path);
        goto skip_read;
    }
    read(fd, buf, sizeof(buf));
    printf(buf);
    skip_read:
    sti();
    for(;;);
}
