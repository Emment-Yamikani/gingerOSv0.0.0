#include <ctype.h>
#include <stdio.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <fs/fs.h>
#include <sys/usr.h>
#include <dev/dev.h>
#include <sys/fcntl.h>
#include <sys/panic.h>
#include <sys/unistd.h>
#include <mm/kmalloc.h>
#include <core/module.h>
#include <fs/ramfs.h>
#include <dev/console/console.h>


char buf[512];


void main()
{
    modules_init();
    vfs_init();
    load_ramdisk();
    kprintemphasis(lcyan, "\n                       Welcome to %sOs-%s\n", OSNAME, OSVERSION);
    kprintemphasis(cyan, "                          Liveliness and energy\n");
    cons_reset();

    kprintprompt(cur_usr->name, cur_usr->cwd);

    printf("\n");

    int ret =0;
    if ((ret = open("osinfo.txt", O_RDONLY)) < 0)
    {
        kprintfailure("could not open file, error code=%d\n", ret);
        goto sti;
    }
    if((ret = read(ret, buf, 511)) < 0){
        kprintfailure("could not read file, error code=%d\n", ret);
        goto sti;
    }

    printf("\n-----------------------------------SYSTEM INFO----------------------------------");
    kprintemphasis(cyan, buf);
    printf("\n--------------------------------------------------------------------------------");
sti:
    sti();
    for(;;);
}
