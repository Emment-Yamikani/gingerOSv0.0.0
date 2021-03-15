#ifndef DCACHE_H
# define DCACHE_H

#include <ds/hash.h>
#include <sys/cdefs.h>

struct inode;

typedef struct dentry
{
    const char   *dname;
    struct inode *dnode;
    struct dentry*dparent;
    hash_node_t  *dchildren;
    int          dcount;
    int          cache_misses;
    int          refs;

    /* dentry flags*/

    int populated :1;
    int sync_pending :1;
    int cached       :1;
}dentry_t;

/*dentry cache for virtual filesystem*/
#define DCACHE_COUNT    17
extern hash_node_t *dcache;


#define DCACHE_MISS 0
#define DCACHE_HIT  1


extern int dlookup (const char *name, mode_t mode);
extern int dcachein (struct dentry *dentry);
extern int duncacheout (struct dentry *dentry);

inline static void dump_dentry(dentry_t *dentry)
{
    if(!dentry)
        return;
    printf("refs=         %d\n", dentry->refs);
    printf("dname=        %s\n", dentry->dname);
    printf("dnode=        %p\n", dentry->dnode);
    printf("cached=       %d\n", dentry->cached);
    printf("dcount=       %d\n", dentry->dcount);
    printf("dparent=      %p\n", dentry->dparent);
    printf("populated=    %d\n", dentry->populated);
    printf("dchildren=    %p\n", dentry->dchildren);
    printf("cache_misses= %d\n", dentry->cache_misses);
    printf("sync_pending= %d\n", dentry->sync_pending);

    /* dentry flags*/

}

#endif