#include <dev\dev.h>
#include <boot\boot.h>
#include <system.h>
#include <string.h>


static uintptr_t ramdev_addr =0;
size_t ramdev_sz =0;

static size_t ramdev_read(struct devid *dev __unused, off_t offset, size_t _size, void *buf)
{
    size_t size = MIN(_size, ramdev_sz - offset);

    memcpy(buf, (void *)ramdev_addr + offset, size);
    return size;
}

static size_t ramdev_write(struct devid *dev __unused, off_t offset, size_t _size, void *buf)
{
    size_t size = MIN(_size, ramdev_sz - offset);

    memcpy((void *)ramdev_addr + offset, buf, size);
    return size;
}

static int ramdev_probe()
{
    ramdev_addr = boot_info.modules[0].addr;
    ramdev_sz = boot_info.modules[0].size;
    kdev_blkdev_register(0, &ramdev);
    return 0;
}

struct dev ramdev = {
    .probe = ramdev_probe,
    .read = ramdev_read,
    .write = ramdev_write,
    .name = "ramdev"
};

MODULE_INIT(ramdev, ramdev_probe, NULL)