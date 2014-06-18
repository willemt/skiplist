
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

static void __free_node(node_t* n)
{
    free(n->right);
    free(n);
}

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

void skiplist_clear(
    skiplist_t * me
)
{
    unsigned int i;
    for (i=0; i<me->levels; i++)
        me->nil->right[i] = NULL;
    me->count = 0;
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

static unsigned int __flip_coins()
{
    int depth;
    for (depth=0; rand() % 2; depth++);
    return depth;
}

static void __swap(node_t* a, node_t* b, unsigned int lvl)
{
    node_t* swp = a->right[lvl];
    a->right[lvl] = b;
    b->right[lvl] = swp;
}

int skiplist_contains_key(
    skiplist_t * me,
    const void *key
)
{
    return (NULL != skiplist_get(me, key));
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
        long c = me->cmp(key, r->ety.k, me->udata);

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

static node_t* __place(
    skiplist_t * me,
    void *key,
    void *val,
    node_t *prev,
    unsigned int *put_depth)
{
    *put_depth = __flip_coins();
    node_t* new = __allocnode(*put_depth);
    __swap(prev, new, 0);
    new->ety.k = key;
    new->ety.v = val;

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

    me->count++;
    return new;
}

static void *__put(
    skiplist_t * me,
    void *key,
    void *val,
    unsigned int lvl,
    node_t *prev,
    unsigned int *put_depth,
    void **v_old
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
                return __place(me, key, val, n, put_depth);

            node_t* placed =  __put(me, key, val, lvl-1, n, put_depth, v_old);

            /* while the stack is rolling back up, we can use the stack to
             * make sure the previous nodes point to the new node correctly. */
            if (placed && (lvl < *put_depth))
                __swap(n, placed, lvl);

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
                r->ety.v = val;
                return n;
            }
            n = r;
        }
        /* straight swap */
        else if (c == 0)
        {
            *v_old = r->ety.v;
            r->ety.v = val;
            return NULL;
        }
    }
}

void *skiplist_put(
    skiplist_t *me,
    void *key,
    void *val
)
{
    if (!key)
        return NULL;

    node_t *n = me->nil;
    unsigned int lvl = me->levels - 1;

    if (!me->nil->right[lvl])
    {
        n = me->head = me->nil->right[lvl] = __allocnode(me->levels);
        n->ety.k = key;
        n->ety.v = val;
        me->count++;
        return NULL;
    }

    unsigned int put_depth;
    void* v = NULL;
    __put(me, key, val, lvl, n, &put_depth, &v);
    return v;
}

static node_t *__remove(
    skiplist_t * me,
    const void *key,
    unsigned int lvl,
    node_t *prev)
{
    node_t *n = prev;

    while (1)
    {
        node_t *r = n->right[lvl];
        long c = me->cmp(key, r->ety.k, me->udata);

        if (0 < c)
        {
            n = r;
        }
        else
        {
            if (0 == lvl)
            {
                if (c == 0)
                {
                    n->right[lvl] = r->right[lvl];
                    return r;
                }
                break;
            }

            node_t* removed = __remove(me, key, lvl-1, prev);
            if (removed && r == removed)
                n->right[lvl] = removed->right[lvl];
            return removed;
        }

        if (!n->right[lvl])
            break;
    }
    return NULL;
}

void *skiplist_remove(
    skiplist_t * me,
    const void *key
)
{
    if (0 == skiplist_count(me) || !key)
        return NULL;

    node_t* removed = __remove(me, key, me->levels - 1, me->nil);
    if (removed)
    {
        void* v = removed->ety.v;
        __free_node(removed);
        me->count--;
        return v;
    }
    return NULL;
}

#if 0
void skiplist_print(skiplist_t *me)
{
    printf("skiplist count: %d:\n", me->count);
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
#endif

/*--------------------------------------------------------------79-characters-*/
