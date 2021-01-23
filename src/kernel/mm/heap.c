#include <mm\alloc_page.h>
#include <mm\vm.h>


typedef union header {
    struct{
        union header * ptr;
        unsigned size;
    }s;
    long _;
}Header;

static Header base;
static Header *freep = (Header*)NULL;

static Header *morecore(size_t);
void * kmalloc(size_t nbytes)
{
    Header *p, *prevp;
    size_t nunits;
    nunits = (nbytes + sizeof(Header)-1) / sizeof(Header) + 1;
    if((prevp = freep) == NULL){
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for(p = prevp->s.ptr;; prevp = p, p = p->s.ptr){
        if(p->s.size >= nunits){
            if (p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else{
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)( p +1);
        }
        if (p == freep)
            if ((p = morecore(nunits)) == NULL)
                return NULL;
    }
}

#define NALLOC 1024
void kfree(void *ap);
static Header * morecore(size_t nu)
{
    char *cp;
    Header *up;
    if (nu < NALLOC)
        nu = NALLOC;
    size_t sz = (nu * sizeof(Header)) / PAGESIZE;
    if ((nu * sizeof(Header)) % PAGESIZE)
        sz++;
    
    cp = (char *)alloc_mapped_contiguous(sz);
    if (cp == (char *)NULL)
        return NULL;
    up = (Header *)cp;
    up->s.size = nu;
    kfree((void *)(up + 1));
    return freep;
}

void *kcalloc(size_t nelem, size_t szelem)
{
    return kmalloc(nelem * szelem);
}

void kfree(void *ap)
{
    Header *bp, *p;
    bp = (Header *)ap -1;
    for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    if (bp + bp->s.size == p->s.ptr){
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
        bp->s.ptr = p->s.ptr;
    if( p + p->s.size == bp){
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else
        p->s.ptr = bp;
    freep = p;
}