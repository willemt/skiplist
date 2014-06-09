#ifndef SKIPLIST_H
#define SKIPLIST_H

typedef long (*func_longcmp_f) (
        const void *k1,
        const void *k2,
        const void *udata);

typedef struct {
    void *k, *v;
} skiplist_entry_t;

typedef struct node_s node_t;

struct node_s
{
    skiplist_entry_t ety;

    /* array of pointers as this node could be on a higher express line level.
     * The current node's line level is determined by remembering the line
     * level of the node which points to this node. That's why we don't record
     * the line level in this struct.
     *
     * We don't record a "left" node because the put() operation backtracks
     * using the * stack via a recursive call */
    node_t **right;
};


typedef struct {
    func_longcmp_f cmp;

    const void* udata;

    /* population within data structure */
    unsigned int count;

    /* number of lines */
    unsigned int levels;

    node_t* nil;

    node_t* head;
} skiplist_t;

/**
 * @param udata User data passed to comparator */
skiplist_t *skiplist_new(func_longcmp_f cmp, const void* udata);

/**
 * Get this key's value.
 * @return key's item, otherwise NULL */
void *skiplist_get(skiplist_t * me, const void *key);

/**
 * @return smallest item */
void *skiplist_get_min(skiplist_t * me);

/**
 * @return largest item */
void *skiplist_get_max(skiplist_t * me);

/**
 * Is this key inside this map?
 * @return 1 if key is in hash, otherwise 0 */
int skiplist_contains_key(skiplist_t * me, const void *key);

/**
 * Remove this key and value from the map.
 * @return value of key, or NULL on failure */
void *skiplist_remove(skiplist_t * me, const void *key);

/**
 * Associate key with val.
 * Does not insert key if an equal key exists.
 * @return previous associated val; otherwise NULL */
void *skiplist_put(skiplist_t * me, void *key, void *val);

/**
 * Remove the value refrenced by this key. */
void skiplist_remove_entry(
    skiplist_t * hmap,
    skiplist_entry_t * entry,
    const void *key
);

/**
 * Put this key/value entry */
void skiplist_put_entry(
    skiplist_t * hmap,
    skiplist_entry_t * entry
);

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
