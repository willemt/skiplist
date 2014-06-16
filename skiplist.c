
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

node_t* __allocnode(skiplist_t* me, unsigned int levels)
{
    node_t* n;

    if (!(n = calloc(1, sizeof(node_t))))
        return NULL;

    if (!(n->right = calloc(1, sizeof(node_t*) * levels)))
        return NULL;

    /* make sure nil and head are included in the new line(s) */
    if (me->levels < levels)
    {
        me->nil->right = realloc(me->nil->right, sizeof(node_t*) * levels);

        unsigned int i;

        for (i=levels-1; me->levels < i ; i--)
        {
            me->nil->right[i] = n;
        }

        me->levels = levels;
    }
    return n;
}

skiplist_t *skiplist_new(func_longcmp_f cmp, const void* userdata)
{
    skiplist_t *me;

    me = calloc(1, sizeof(skiplist_t));
    me->udata = userdata;
    me->cmp = cmp;
    me->levels = 1;
    me->nil = __allocnode(me, me->levels);
    return me;
}

int skiplist_count(const skiplist_t * me)
{
    return me->count;
}

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

void skiplist_clear(
    skiplist_t * me
)
{
    // TODO
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
    node_t *r;

    while (1)
    {
        /* special case for head */
        if (n == me->head)
        {
            r = n;
            goto insert;
        }

        r = n->right[lvl];
        long c = me->cmp(key, r->ety.k, NULL);

        /* we are bigger, move onwards */
        if (0 < c)
        {
            n = r;
        }
        /* straight swap */
        else if (c == 0)
        {
            void* old_v = r->ety.v;
            r->ety.v = val_new;
            return NULL;
        }
        /* move down a lane */
        else
        {
            /* if we're on the bottom lane, we've found our spot */
            if (lvl == 0)
                goto insert;

            node_t* inserted =  __put(me, key, val_new, lvl-1, r, put_depth);

            /* while the stack is rolling back up, we can make sure the
             * previous nodes point to the new node correctly */
            int i;
            for (i=lvl; inserted && i <= *put_depth; i++)
            {
                n->right[i] = inserted;
                inserted->right[i] = n->right[i];
            }
            return inserted;
        }
    }

insert:
    *put_depth = __flip_coins();
    n->right[lvl] = __allocnode(me, *put_depth);
    return n;
}

void *skiplist_put(
    skiplist_t * me,
    void *key,
    void *val_new
)
{
    if (!key)
        return NULL;

    unsigned int lvl = me->levels - 1;
    unsigned int put_depth;

    node_t *n = me->nil;

    return __put(me, key, val_new, lvl, n, &put_depth);
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

    return (void *) e.v;
}

/*--------------------------------------------------------------79-characters-*/
