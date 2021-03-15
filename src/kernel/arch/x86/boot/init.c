#include <stdio.h>
#include <mm/vm.h>
#include <mm/pm.h>
#include <fs/fs.h>
#include <sys/usr.h>
#include <dev/dev.h>
#include <boot/boot.h>
#include <fs/ramfs.h>
#include <dev/console/console.h>
#include <arch/x86/chipset/chipset.h>

boot_t boot_info;
extern void main();
extern void init_cpu();


void early_init(multiboot_info_t *info)
{
    init_cpu();
    get_multiboot_info(info);
    pmm_setup(boot_info.pm_size);
    setup_x86_paging();
    init_chipset();
    init_console();
    main();
    for(;;);
}