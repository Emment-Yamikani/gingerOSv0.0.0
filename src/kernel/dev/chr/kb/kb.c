#include "kbd.h"
#include <sys\errno.h>
#include <string.h>
#include <system.h>
#include <stdio.h>
#include <dev\dev.h>
#include <core\module.h>
#include <arch\x86\cpu\isrs.h>
#include <arch\x86\cpu\x86io.h>
#include <dev\console\console.h>

struct dev ps2kbd;
#define MAX_KBD 512

static struct{
    char buf[MAX_KBD];
    int pos;
}ps2kbd_buf;



void ps2kbd_init()
{
    memset((void *)&ps2kbd_buf, 0, sizeof(ps2kbd_buf));
}

int csr_bkwds =0;

void ps2kbd_handler(void *arch __unused)
{
    if(ps2kbd_buf.pos >= MAX_KBD)
        ps2kbd_buf.pos =0;

    static uint32_t shift;
    static uint8_t *charcode[4] = {
        normalmap, shiftmap, ctlmap, ctlmap};
    uint8_t st, data, c;

    st = inb(KBSTATP);
    if ((st & KBS_DIB) == 0)
        return;

    data = inb(KBDATAP);

    if (data == 0xE0)
    {
        shift |= E0ESC;
        return;
    }
    else if (data & 0x80)
    {
        // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);
        return;
    }
    else if (shift & E0ESC)
    {
        // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];
    if (shift & CAPSLOCK)
    {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }

    switch(c)
    {
        case KEY_LF:{
            if(console_x != 0)
                console_x--;
            else {
                console_x =79;
                if (console_y != 0)
                    console_y--;
            }
            csr_bkwds++;
            console_set_csr(console_getscrn_offset(console_x, console_y));
            break;
        }
        case KEY_RT:{
            if(!csr_bkwds)
                break;
            csr_bkwds--;
            if(console_x == 79){
                console_x =0;
                if(console_y < 25)
                    console_y++;
            }
            else console_x++;
            console_set_csr(console_getscrn_offset(console_x, console_y));
            break;
        }
        default:{
            ps2kbd_buf.buf[ps2kbd_buf.pos++] = c;
            if (c == '\b'){
                ps2kbd_buf.buf[--ps2kbd_buf.pos] = 0;
            }
            if (csr_bkwds && (c != '\b'))
                csr_bkwds--;
                console_write((char *)&c, 1);
            break;
        }
    };
}

void ps2kbd_register()
{
    kdev_chrdev_register(1, &ps2kbd);
}

int ps2kbd_probe()
{
    ps2kbd_init();
    install_isr_handler(irq1_Keyboard, ps2kbd_handler);
    ps2kbd_register();
    return 0;
}

size_t ps2kbd_read(struct devid *dev __unused, off_t offset, size_t _size, void *buf)
{
    size_t size = MIN(_size, MAX_KBD - offset);
    memcpy(buf, ps2kbd_buf.buf + offset, size);
    return size;
}

int ps2kbd_open(struct file *file __unused, mode_t mode __unused)
{
    kprintfailure("ps2kbd open not ready\n");
    return -EINVAL;
}

int ps2kbd_fread(struct file *file, void *buf, size_t _size)
{
    if(file->foffset >= MAX_KBD)
        return (-1);
    size_t size = MIN(_size, MAX_KBD);
    memcpy(buf, ps2kbd_buf.buf + file->foffset, size);
    return size;
}

int ps2kbd_close(struct file *file __unused)
{
    kprintfailure("ps2kbd close not ready\n");
    return -EINVAL;
}

struct dev ps2kbd = {
    .name = "ps2kbd",
    .probe = ps2kbd_probe,
    .read = ps2kbd_read,
};

MODULE_INIT(ps2kbd, ps2kbd_probe, NULL)