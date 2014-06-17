
/**
 * Copyright (c) 2011, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. 
 *
 * @file
 * @author  Willem Thiart himself@willemthiart.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <assert.h>

#include "skiplist.h"


#if 0
static void __support_new_lines(skiplist_t* me, node_t* new, unsigned int levels)
{
}
#endif

static node_t* __allocnode(unsigned int levels)
{
    node_t* n;

    if (!(n = calloc(1, sizeof(node_t))))
        return NULL;
    if (!(n->right = calloc(1, sizeof(node_t*) * levels)))
        return NULL;
    return n;
}

skiplist_t *skiplist_new(func_longcmp_f cmp, const void* userdata)
{
    skiplist_t *me;

    me = calloc(1, sizeof(skiplist_t));
    me->udata = userdata;
    me->cmp = cmp;
    me->levels = 1;
    me->nil = __allocnode(me->levels);
    return me;
}

int skiplist_count(const skiplist_t * me)
{
    return me->count;
}

#if 0
/**
 * free all the nodes in a chain, recursively. */
static void __node_empty(
    skiplist_t * me,
    node_t * node
)
{
    if (NULL == node)
    {
        return;
    }
    else
    {
//        __node_empty(me, node->next);
        free(node);
        me->count--;
    }
}
#endif

void skiplist_clear(
    skiplist_t * me
)
{
    unsigned int i;
    for (i=0; i<me->levels; i++)
        me->nil->right[i] = NULL;
}

void skiplist_free(
    skiplist_t * me
)
{
    skiplist_clear(me);
}

void skiplist_freeall(
    skiplist_t * me
)
{
    skiplist_free(me);
    free(me);
}

void *skiplist_get(skiplist_t * me, const void *key)
{
    if (0 == skiplist_count(me) || !key)
        return NULL;

    int lvl = me->levels - 1;
    node_t *n = me->nil;

    while (0 <= lvl && n != me->head)
    {
        node_t *r = n->right[lvl] ? n->right[lvl] : me->head;
        long c = me->cmp(key, r->ety.k, NULL);

        if (c < 0)
        {
            lvl -= 1;
        }
        else if (0 < c)
        {
            n = r;
        }
        else
        {
            return r->ety.v;
        }
    }
    return NULL;
}

static unsigned int __flip_coins()
{
    int depth;
    for (depth=0; rand() % 2; depth++);
    return depth;
}

static node_t* __place(
    skiplist_t * me,
    void *key,
    void *val_new,
    node_t *prev,
    unsigned int *put_depth)
{
    *put_depth = __flip_coins();
    node_t* swp = prev->right[0];
    node_t* new = prev->right[0] = __allocnode(*put_depth);
    new->right[0] = swp;
    new->ety.k = key;
    new->ety.v = val_new;

    printf("placed %d me->level:%d\n", *put_depth, me->levels);

    /* make sure nil is included in the new line(s) */
    if (me->levels < *put_depth)
    {
        me->nil->right = realloc(me->nil->right, sizeof(node_t*) * *put_depth);
        me->head->right = realloc(me->head->right, sizeof(node_t*) * *put_depth);

        unsigned int i;
        for (i=*put_depth - 1; me->levels - 1 < i; i--)
        {
            me->nil->right[i] = new;
            if (me->head != new)
                new->right[i] = me->head;
            me->head->right[i] = NULL;
        }

        me->levels = *put_depth;
    }
    return new;
}

static void *__put(
    skiplist_t * me,
    void *key,
    void *val_new,
    unsigned int lvl,
    node_t *prev,
    unsigned int *put_depth
)
{
    node_t *n = prev;

    while (1)
    {
        node_t *r = n->right[lvl];
        long c = me->cmp(key, r->ety.k, me->udata);

        /* we are smaller, move down a lane */
        if (c < 0)
        {
            /* if we're on the bottom lane, we've found our spot */
            if (lvl == 0)
                return __place(me, key, val_new, n, put_depth);

            node_t* placed =  __put(me, key, val_new, lvl-1, r, put_depth);

            /* while the stack is rolling back up, we can use the stack to
             * make sure the previous nodes point to the new node correctly. */
            if (placed && (lvl <= *put_depth))// || placed == me->head))
            {
                node_t* swp = n->right[lvl];
                n->right[lvl] = placed;
                placed->right[lvl] = swp;
            }
            return placed;
        }
        /* we are larger, move onwards */
        else if (0 < c)
        {
            if (r == me->head)
            {
                /* head get's placed instead */
                n = __place(me, r->ety.k, r->ety.v, n, put_depth);
                r->ety.k = key;
                r->ety.v = val_new;
                return n;
            }
            n = r;
        }
        /* straight swap */
        else if (c == 0)
        {
            //void* old_v = r->ety.v;
            r->ety.v = val_new;
            return NULL;
        }
    }
}

void skiplist_print(skiplist_t *me)
{
    printf("skiplist:\n");
    unsigned int i;
    for (i=me->levels-1; 1; i--)
    {
        printf("%d|", i);
        node_t* n = me->nil->right[i];
        while (n)
        {
            printf("k:%d", (unsigned int)n->ety.k);
            printf("v:%d ", (unsigned int)n->ety.v);
            n = n->right[i];
        }
        printf("\n");
        if (i==0) break;
    }
    printf("\n");
}

void *skiplist_put(
    skiplist_t *me,
    void *key,
    void *val_new
)
{
    if (!key)
        return NULL;

    node_t *n = me->nil;
    unsigned int lvl = me->levels - 1;

    printf("k:%d", (unsigned int)key);
    printf("v:%d ", (unsigned int)val_new);


    if (!me->nil->right[lvl])
    {
        n = me->head = me->nil->right[lvl] = __allocnode(me->levels);
        n->ety.k = key;
        n->ety.v = val_new;
    skiplist_print(me);
        return NULL;
    }

    unsigned int put_depth;
    n = __put(me, key, val_new, lvl, n, &put_depth);
    skiplist_print(me);
    return n;
}

int skiplist_contains_key(
    skiplist_t * me,
    const void *key
)
{
    return (NULL != skiplist_get(me, key));
}

void *skiplist_remove(
    skiplist_t * me,
    const void *key
)
{
    skiplist_entry_t e;

    printf("%d\n", me->levels);
    printf("%p\n", key);

    return (void *) e.v;
}

/*--------------------------------------------------------------79-characters-*/
