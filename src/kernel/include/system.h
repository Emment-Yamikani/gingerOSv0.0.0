#ifndef SYSTEM_H
# define SYSTEM_H


#define OSNAME "ginger"
#define OSVERSION "v0.0.2"

#ifndef DEC
#define DEC 0
#endif
#ifndef BIN
#define BIN 1
#endif
#ifndef OCT
#define OCT 2
#endif
#ifndef HEX
#define HEX 3
#endif

#define SHL(i, c) ((i << c))
#define SHR(i, c) ((i >> c))
#define _BS(i) ((1 << i))

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define __unused __attribute__((unused))
#define __section(s) __attribute__((section(s)))


static inline int __always() { return 1; }
static inline int __never() { return 0; }

#define __CAT(a, b) a##b
#define MODULE_INIT(name, i, f)                            \
    __section(".__minit") void *__CAT(__minit_, name) = i; \
    __section(".__mfini") void *__CAT(__mfini_, name) = f;

#define foreach(element, list) \
    for (typeof(*list) *tmp = list, element = *tmp; element; element = *++tmp)

#define forlinked(element, list, iter) \
    for (typeof(list) element = list; element; element = iter)

extern void init_tasking();
static inline void cli()
{
    asm volatile("cli");
}

static inline void sti()
{
    asm volatile("sti");
}

extern void kprintprompt(const char *usr, const char *cwd);
extern void _KprintOK();
extern int kprintfOKmsg(const char *fmt, ...);
extern int kprintfailure(const char *fmt, ...);

#endif