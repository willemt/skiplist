#ifndef SKIPLIST_H
#define SKIPLIST_H

typedef long (*func_longcmp_f) (
        const void *k1,
        const void *k2,
        const void *udata);

typedef struct {
    void *k, *v;
} entry_t;

typedef struct node_s node_t;

struct node_s
{
    entry_t ety;

    /* array of pointers as this node could be on a higher express line level.
     * The current node's line level is determined by remembering the line
     * level of the node which points to this node. That's why we don't record
     * the line level in this struct */
    node_t **right;
};


typedef struct {
    func_longcmp_f cmp;

    void* udata;

    int count;

    /* number of levels */
    int levels;

    node_t* nil;
    node_t* head;
} skiplist_t;

skiplist_t *skiplist_new(func_longcmp_f cmp);

void *skiplist_get(skiplist_t * me, const void *key);

int skiplist_contains_key(skiplist_t * me, const void *key);

void skiplist_remove_entry(skiplist_t * me, entry_t * e, const void *key);

void *skiplist_remove(skiplist_t * me, const void *key);

void *skiplist_put(skiplist_t * me, void *key, void *val);

void skiplist_put_entry(skiplist_t * me, entry_t * e);

/**
 * @return number of items */
int skiplist_count(const skiplist_t * me);

/**
 * Remove all items */
void skiplist_clear(skiplist_t * me);

void skiplist_free(skiplist_t * me);

void skiplist_freeall(skiplist_t * me);
/*--------------------------------------------------------------79-characters-*/

#endif /* SKIPLIST_H */
