#ifndef SYS_HASH_H
# define SYS_HASH_H
#include <stdio.h>
#include <string.h>
#include <sys/panic.h>
#include <mm/kmalloc.h>

typedef struct hash{
    int key;
    int used:1;
    void *value; //generic data pointer
    struct hash *prev;
    struct hash *next;
}hash_node_t;

static inline void hash_dump_node(hash_node_t *node)
{
    if(!node){
        printf("(nul)");
    return;}
    printf("(%d)%p->", node->key, node->value);
    hash_dump_node(node->next);
}

static inline int hash_func(int key, int nelems)
{
    key += nelems;
    key %= nelems;
    return key;
}

static inline int mkhashkey_str(const char *s)
{
    /**
     * !this is either a smart or really stupid implementation*
     * */

    int len = strlen(s);
    char *str = (char *)s;
    int sum = 0;
    for (int i = 0, count = 0; i < len; count = ++i)
        while (count--)
            sum += str[i];
    while (*str)
        sum += (int)*str++;
    return sum;
}

static inline hash_node_t *hash_search(hash_node_t *table, int nelems, int key)
{
    int i = hash_func(key, nelems);
    hash_node_t * tmp = &table[i];
    while(tmp)
    {
        if(tmp->key == key)
            return tmp;
        tmp = tmp->next;
    }
    return 0;
}

static inline void hash_insert(hash_node_t *table, int nelems, int key, void *value)
{
    int i = hash_func(key, nelems);
    if(!table[i].used){
        table[i].value = value;
        table[i].key = key;
        table[i].used = 1;
    }
    else{
        hash_node_t * node = (hash_node_t *)kmalloc(sizeof(*node));
        if(!node)
            panic("hash.h: exit(ENOMEM)");
        node->value = value;
        node->key = key;
        node->next =0;
        node->used =1;
        hash_node_t *tmp = &table[i];
        while(tmp->next)
            tmp = tmp->next;
        tmp->next = node;
        node->prev = tmp;
    }
}

static inline void hash_delete(hash_node_t * node)
{
    if(node){
        hash_node_t *x = node->next;
        if(x){
            void *xdata = node->value;
            node->value = x->value;
            node->key = x->key;
            node->next = x->next;
            node->used = x->used;
            kfree(xdata);
            kfree((void *)x);
        }
        else{
            if(node->prev){
                node->prev->next = 0;
                kfree((void *)node->value);
                kfree((void *)node);
            }
        }
    }
}

static inline hash_node_t *mkhash_table(int nelems)
{
    hash_node_t *table = (hash_node_t *)kcalloc(nelems, sizeof(*table));
    if(!table)
        panic("hash.h: exit(ENOMEM)");
    memset((void *)table, 0, (nelems * sizeof(*table)));
    return table;
}

#endif