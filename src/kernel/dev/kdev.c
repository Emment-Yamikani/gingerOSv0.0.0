#include <stdio.h>
#include <system.h>
#include <dev\dev.h>
#include <sys\errno.h>

static struct dev *chrdev[256] = {0};
static struct dev *blkdev[256] = {0};

static inline struct dev *kdev_get(struct devid *dd)
{
    struct dev *dev = NULL;

    switch (dd->type)
    {
    case CHRDEV:
        dev = chrdev[dd->major];
        break;
    case BLKDEV:
        dev = blkdev[dd->major];
        break;
    }

    return dev;
}

int kdev_open(struct devid *dd __unused)
{
    return __always();
}

int kdev_close(struct devid *dev __unused)
{
    return 0;
}
size_t kdev_read(struct devid *dd, off_t offset, size_t size, void *buf)
{
    struct dev *dev = NULL;

    if (!(dev = kdev_get(dd)))
        return -ENXIO;

    if (!dev->read)
        return -ENXIO;

    return dev->read(dd, offset, size, buf);
}

size_t kdev_write(struct devid *dd, off_t offset, size_t size, void *buf)
{
    struct dev *dev = NULL;

    if (!(dev = kdev_get(dd)))
        return -ENXIO;

    if (!dev->write)
        return -ENXIO;

    return dev->write(dd, offset, size, buf);
}

void kdev_chrdev_register(devid_t major, struct dev *dev)
{
    chrdev[major] = dev; /* XXX */
    kprintfOKmsg("kdev: registered chrdev %d: %s\n", major, dev->name);
}

void kdev_blkdev_register(devid_t major, struct dev *dev)
{
    blkdev[major] = dev; /* XXX */
    kprintfOKmsg("kdev: registered blkdev %d: %s\n", major, dev->name);
}

void kdev_init()
{
    kprintfOKmsg("kdev: initializing...\n");
}