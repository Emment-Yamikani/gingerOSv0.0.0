#include <fs/fs.h>
#include <string.h>
#include <system.h>
#include <stdbool.h>
#include <dev/dev.h>
#include <sys/mutex.h>
#include <arch/x86/cpu/x86io.h>

#define max_cols 80
#define max_rows 25
#define ctrl_reg 0x3d4
#define data_reg 0x3d5

#define console_buffsz (2*(max_cols * max_rows))

#define black 0x00
#define blue 0x01
#define green 0x02
#define cyan 0x03
#define red 0x04
#define magenta 0x05
#define pink magenta
#define brown 0x06
#define lgrey 0x07
#define grey 0x08
#define lblue 0x09
#define lgreen 0x0A
#define lcyan 0x0B
#define lred 0x0C
#define lmagenta 0x0D
#define lbrown 0x0E
#define white 0x0F

typedef unsigned char color_t;
int console_x, console_y, offset;
static char attrib;
static unsigned short *console_buf;
int scrollf= 0;
color_t default_backg, default_foreg,
cur_foreg, cur_backg, prev_foreg, prev_backg;

struct spinlock console_lock;
static int pos =0;


int console_getcsr_offset()
{
    int offset;
    outb(ctrl_reg, 14);
    offset = (inb(data_reg)) << 8;
    outb(ctrl_reg, 15);
    return (offset += inb(data_reg)) * 2;
}


int console_getscrn_offset(int x, int y)
{
    return (max_cols * y + x);
}

void console_set_csr(int offset)
{
    outb(ctrl_reg, 14);
    outb(data_reg, (char)(offset >> 8));
    outb(ctrl_reg, 15);
    outb(data_reg, (char)offset);
}

static void console_scrl()
{
    if (!scrollf)
        return;
    unsigned short blank = (attrib << 8) | ' ';
    unsigned short buf_sz = 2 * (max_cols * max_rows);
    if (console_y >= max_rows)
    {
        memcpy(console_buf + console_getscrn_offset(0, 0), console_buf + console_getscrn_offset(0, 1), buf_sz);
        memsetw(console_buf + console_getscrn_offset(0, 24), blank, max_cols);
        console_x = 0, console_y = 24;
    }
}

static void console_putc(char c)
{
    if (c == '\n')
    {
        ++console_y;
        if (console_y >= max_rows)
        {
            console_scrl();
            console_y = max_rows - 1;
        }
        console_x = 0;
        console_set_csr(offset = console_getscrn_offset(console_x, console_y));
    }
    else if (c == '\b')
    {
        if (console_y > 0)
        {
            --console_x;
            if (console_x < 0)
            {
                console_x = max_cols - 1;
                --console_y;
            }
        }
        else if (console_y == 0)
        {
            --console_x;
            if (console_x < 0)
                console_x = 0;
        }
        console_buf[console_getscrn_offset(console_x, console_y)] = (attrib << 8) | ' ';
        console_set_csr(offset = console_getscrn_offset(console_x, console_y));
    }
    else if (c == '\t')
    {
        console_x = (console_x + 4) & ~(4 - 1);
        console_set_csr(offset = console_getscrn_offset(console_x, console_y));
    }
    else if (c == '\r')
    {
        console_x = 0;
        console_set_csr(offset = console_getscrn_offset(console_x, console_y));
    }
    else if (c >= ' ')
    {
        console_buf[console_getscrn_offset(console_x, console_y)] = (attrib << 8) | c;
        ++console_x;
        if (console_x >= max_cols)
        {
            console_x = 0;
            ++console_y;
            if (console_y >= max_rows)
                console_scrl();
        }
        console_set_csr(console_getscrn_offset(console_x, console_y));
    }
    console_scrl();
}


int console_write(char *cbuf, int size)
{
    int len =0;
    while(cbuf[len] && len < size)
        console_putc(cbuf[len++]);
    return len;
}

void console_clrs()
{
    console_x = console_y = 0;
    scrollf = false;
    for (int i = 0; i < (2 * (max_rows * max_cols)); i++)
        console_putc(' ');
    console_x = console_y = 0;
    console_set_csr(console_getscrn_offset(console_x, console_y));
    scrollf = true;
}


void cons_setcolor(color_t back, color_t fore)
{
    attrib = (back << 4) | fore;
}

void cons_setforecolor(color_t foreg)
{
    prev_foreg = cur_foreg;
    cur_foreg = foreg;
}

void cons_setbackcolor(color_t backg)
{
    prev_backg = cur_backg;
    cur_backg = backg;
}

void cons_restore_color()
{
    cur_backg = prev_backg;
    cur_foreg = prev_foreg;
    cons_setcolor(prev_backg, prev_foreg);
}

void cons_update_color()
{
    cons_setcolor(cur_backg, cur_foreg);
}

int console_getcolor()
{
    return attrib;
}

static int tmpx, tmpy;
void console_savecord()
{
    tmpx = console_x, tmpy = console_y;
}

void console_restorecord()
{
    console_x = tmpx, console_y = tmpy;
}

void init_console()
{
    console_buf = (unsigned short *)0xc00b8000;
    cons_setcolor(black, white);
    default_foreg = cur_foreg = white;
    default_backg = cur_backg = black;
    offset = console_y = console_x = 0;
    console_clrs();
}

int cons_probe()
{
    kdev_chrdev_register(0, &consdev);
    return 0;
}

size_t cons_fwrite(struct file *file, void *buf, size_t _size)
{
    size_t size = MIN(_size, console_buffsz - file->foffset);
    size = (size_t)console_write((char *)buf, size);
    return size;
}

int cons_fclose(struct file *file)
{

}


struct dev consdev = {
    .name = "console",
    .probe = cons_probe,
    .read = __never,
    .write = console_write,
    .fops = {
        .fopen = __never,
        .fread = __never,
        .fwrite = cons_fwrite,
        .fclose = cons_fclose
    }
};

MODULE_INIT(consdev, cons_probe, NULL);