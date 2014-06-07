
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

static void __ensurecapacity(
    skiplist_t * me
);

/**
 * Allocate memory for nodes. Used for chained nodes. */
static node_t *__allocnodes(
    skiplist_t * me,
    unsigned int count
)
{
    // FIXME: make a chain node reservoir
    return calloc(count, sizeof(node_t));
}

node_t* __allocnode(int levels)
{
    node_t* n = calloc(1, sizeof(node_t));
    n->right = calloc(1, sizeof(node_t*) * levels);
    return n;
}

skiplist_t *skiplist_new(func_longcmp_f cmp)
{
    skiplist_t *me;

    me = calloc(1, sizeof(skiplist_t));
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

}

void skiplist_free(
    skiplist_t * me
)
{
    assert(me);
    skiplist_clear(me);
}

void skiplist_freeall(
    skiplist_t * me
)
{
    assert(me);
    skiplist_free(me);
    free(me);
}

void *skiplist_get(skiplist_t * me, const void *key)
{
    if (0 == skiplist_count(me) || !key)
        return NULL;

    int lvl = me->levels - 1;
    node_t *n = me->nil;

    while (0 <= lvl)
    {
        if (!n)
            return NULL;

        node_t* r = n->right[lvl];
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

void *skiplist_put(
    skiplist_t * me,
    void *key,
    void *val_new
)
{
    if (!key)
        return NULL;

    int lvl = me->levels - 1;
    node_t *n = me->nil;

    while (0 <= lvl)
    {
        if (!n->right)
        {
            break;
        }

        node_t* r = n->right[lvl];
        long c = me->cmp(key, r->ety.k, NULL);

        if (c < 0)
        {
            lvl -= 1;
        }
        else if (0 < c)
        {
            int i;
            for (i=0; rand() % 2; i++);

            n->right[lvl] = __allocnode(me, i);
            n->right[lvl]->right[lvl] = r;
        }
        else
        {
            return r->ety.v;
        }
    }

    return NULL;
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
