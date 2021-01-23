#ifndef CONSOLE_H
# define CONSOLE_H


typedef unsigned char color_t;
#ifndef black
#define black 0x00
#endif
#ifndef blue
#define blue 0x01
#endif
#ifndef green
#define green 0x02
#endif
#ifndef cyan
#define cyan 0x03
#endif
#ifndef red
#define red 0x04
#endif
#ifndef magenta
#define magenta 0x05
#endif
#ifndef pink
#define pink magenta
#endif
#ifndef brown
#define brown 0x06
#endif
#ifndef lgrey
#define lgrey 0x07
#endif
#ifndef grey
#define grey 0x08
#endif
#ifndef lblue
#define lblue 0x09
#endif
#ifndef lgreen
#define lgreen 0x0A
#endif
#ifndef lcyan
#define lcyan 0x0B
#endif
#ifndef lred
#define lred 0x0C
#endif
#ifndef lmagenta
#define lmagenta 0x0D
#endif
#ifndef lbrown
#define lbrown 0x0E
#endif
#ifndef white
#define white 0x0F
#endif

extern int console_x, console_y, offset;
extern void init_console();
extern void console_clrs();
extern int console_getcsr_offset();
extern void console_set_csr(int offset);
extern int console_getscrn_offset(int x, int y);
extern int console_write(char *buf, int size);
extern void cons_setcolor(color_t back, color_t fore);
extern void cons_setforecolor(color_t foreg);
extern void cons_setbackcolor(color_t backg);
extern void cons_update_color();
extern void cons_restore_color();

extern void cons_reset();
extern void cons_black();
extern void cons_blue();
extern void cons_green();
extern void cons_cyan();
extern void cons_red();
extern void cons_magenta();
extern void cons_pink();
extern void cons_brown();
extern void cons_lgrey();
extern void cons_grey();
extern void cons_lblue();
extern void cons_lgreen();
extern void cons_lcyan();
extern void cons_lred();
extern void cons_lmagenta();
extern void cons_lbrown();
extern void cons_white();


#endif