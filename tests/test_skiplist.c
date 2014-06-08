#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "linked_list_skiplist.h"

static unsigned long __uint_hash(
    const void *e1
)
{
    const long i1 = (unsigned long) e1;

    assert(i1 >= 0);
    return i1;
}

static long __uint_compare(
    const void *e1,
    const void *e2
)
{
    const long i1 = (unsigned long) e1, i2 = (unsigned long) e2;

//      return !(*i1 == *i2); 
    return i1 - i2;
}

void Testskiplistlinked_New(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__uint_compare, NULL);

    CuAssertTrue(tc, 0 == skiplist_count(d));
    CuAssertTrue(tc, 11 == skiplist_size(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_Put(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__uint_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_PutEnsuresCapacity(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__uint_hash, __uint_compare, 1);
    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 51, (void *) 92);

    CuAssertTrue(tc, 2 == skiplist_count(d));
    CuAssertTrue(tc, 2 == skiplist_size(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_PutHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 93);

    CuAssertTrue(tc, 2 == skiplist_count(d));

    val = (unsigned long) skiplist_get(d, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);

    val = (unsigned long) skiplist_get(d, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    skiplist_freeall(d);
}

void Testskiplistlinked_GetHandlesCollisionByTraversingChain(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 93);

    CuAssertTrue(tc, 2 == skiplist_count(d));

    val = (unsigned long) skiplist_get(d, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);
    skiplist_freeall(d);
}

void Testskiplistlinked_RemoveReturnsNullIfMissingAndTraversesChain(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    skiplist_put(d, (void *) 1, (void *) 92);

    val = (unsigned long) skiplist_remove(d, (void *) 5);
    CuAssertTrue(tc, 0 == val);

    val = (unsigned long) skiplist_remove(d, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);

    skiplist_freeall(d);
}

void Testskiplistlinked_RemoveHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 93);
    skiplist_put(d, (void *) 9, (void *) 94);

    val = (unsigned long) skiplist_remove(d, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);
    CuAssertTrue(tc, 2 == skiplist_count(d));

    val = (unsigned long) skiplist_remove(d, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    CuAssertTrue(tc, 1 == skiplist_count(d));

    skiplist_freeall(d);
}

void Testskiplistlinked_PutEntry(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_entry_t entry;

    d = skiplist_new(__uint_compare, NULL);
    entry.key = (void *) 50;
    entry.val = (void *) 92;
    skiplist_put_entry(d, &entry);
    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_Get(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    val = (unsigned long) skiplist_get(d, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    skiplist_freeall(d);
}

void Testskiplistlinked_ContainsKey(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == skiplist_contains_key(d, (void*) 50));
    skiplist_freeall(d);
}


void Testskiplistlinked_DoublePut(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 50, (void *) 23);
    val = (unsigned long) skiplist_get(d, (void *) 50);
    CuAssertTrue(tc, val == 23);
    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_Get2(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 49, (void *) 91);
    skiplist_put(d, (void *) 48, (void *) 90);
    skiplist_put(d, (void *) 47, (void *) 89);
    skiplist_put(d, (void *) 46, (void *) 88);
    skiplist_put(d, (void *) 45, (void *) 87);
    val = (unsigned long) skiplist_get(d, (void *) 48);
    CuAssertTrue(tc, val == 90);
    skiplist_freeall(d);
}

void Testskiplistlinked_IncreaseCapacityDoesNotBreakskiplist(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    skiplist_put(d, (void *) 1, (void *) 90);
    skiplist_put(d, (void *) 5, (void *) 91);
    skiplist_put(d, (void *) 2, (void *) 92);
    skiplist_increase_capacity(d,2);
    CuAssertTrue(tc, 3 == skiplist_count(d));
    val = (unsigned long) skiplist_get(d, (void *) 1);
    CuAssertTrue(tc, val == 90);
    val = (unsigned long) skiplist_get(d, (void *) 5);
    CuAssertTrue(tc, val == 91);
    val = (unsigned long) skiplist_get(d, (void *) 2);
    CuAssertTrue(tc, val == 92);
    skiplist_freeall(d);
}

void Testskiplistlinked_Remove(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    val = (unsigned long) skiplist_remove(d, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplistlinked_ClearRemovesAll(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 2, (void *) 102);
    skiplist_clear(d);

    val = (unsigned long) skiplist_get(d, (void *) 1);
    CuAssertTrue(tc, 0 == val);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

/* The clear function will need to clean the whole chain */
void Testskiplistlinked_ClearHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__uint_hash, __uint_compare, 4);

    skiplist_put(d, (void *) 1, (void *) 50);
    /* all of the rest cause collisions */
    skiplist_put(d, (void *) 5, (void *) 51);
    skiplist_put(d, (void *) 9, (void *) 52);
    skiplist_clear(d);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

#if 0
void T_estskiplistlinked_DoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_iterator_t iter;

    d = skiplist_new(__uint_compare, NULL);
    skiplist_iterator(d, &iter);
    CuAssertTrue(tc, 0 == skiplist_iterator_has_next(d, &iter));
    skiplist_freeall(d);
}

void T_estskiplistlinked_RemoveItemDoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_iterator_t iter;

    d = skiplist_new(__uint_compare, NULL);
    skiplist_put(d, (void *) 9, (void *) 52);
    skiplist_remove(d, (void *) 9);
    skiplist_iterator(d, &iter);
    CuAssertTrue(tc, 0 == skiplist_iterator_has_next(d, &iter));
    skiplist_freeall(d);
}

void T_estskiplistlinked_Iterate(
    CuTest * tc
)
{
    skiplist_t *d;

    skiplist_t *d2;

    skiplist_iterator_t iter;

    void *key;

    d = skiplist_new(__uint_compare, NULL);
    d2 = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 49, (void *) 91);
    skiplist_put(d, (void *) 48, (void *) 90);
    skiplist_put(d, (void *) 47, (void *) 89);
    skiplist_put(d, (void *) 46, (void *) 88);
    skiplist_put(d, (void *) 45, (void *) 87);

    skiplist_put(d2, (void *) 50, (void *) 92);
    skiplist_put(d2, (void *) 49, (void *) 91);
    skiplist_put(d2, (void *) 48, (void *) 90);
    skiplist_put(d2, (void *) 47, (void *) 89);
    skiplist_put(d2, (void *) 46, (void *) 88);
    skiplist_put(d2, (void *) 45, (void *) 87);

    skiplist_iterator(d, &iter);

    /*  remove every key we iterate on */
    while ((key = skiplist_iterator_next(d, &iter)))
    {
        CuAssertTrue(tc, NULL != skiplist_remove(d2, key));
    }

    /*  check if the skiplist is empty */
    CuAssertTrue(tc, 0 == skiplist_count(d2));
    skiplist_freeall(d);
}

void T_estskiplistlinked_IterateHandlesCollisions(
    CuTest * tc
)
{
    skiplist_t *d, *d2;

    skiplist_iterator_t iter;

    void *key;

    d = skiplist_new(__uint_hash, __uint_compare, 4);
    d2 = skiplist_new(__uint_hash, __uint_compare, 4);

    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 91);
    skiplist_put(d, (void *) 9, (void *) 90);

    skiplist_put(d2, (void *) 1, (void *) 92);
    skiplist_put(d2, (void *) 5, (void *) 91);
    skiplist_put(d2, (void *) 9, (void *) 90);

    skiplist_iterator(d, &iter);

    /*  remove every key we iterate on */
    while ((key = skiplist_iterator_next(d, &iter)))
    {
        CuAssertTrue(tc, NULL != skiplist_remove(d2, key));
    }

    /*  check if the skiplist is empty */
    CuAssertTrue(tc, 0 == skiplist_count(d2));
    skiplist_freeall(d);
}

void T_estskiplistlinked_IterateAndRemoveDoesntBreakIteration(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_t *d2;
    skiplist_iterator_t iter;
    void *key;

    d = skiplist_new(__uint_compare, NULL);
    d2 = skiplist_new(__uint_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 49, (void *) 91);
    skiplist_put(d, (void *) 48, (void *) 90);
    skiplist_put(d, (void *) 47, (void *) 89);
    skiplist_put(d, (void *) 46, (void *) 88);
    skiplist_put(d, (void *) 45, (void *) 87);
    /*  the following 3 collide: */
    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 91);
    skiplist_put(d, (void *) 9, (void *) 90);

    skiplist_put(d2, (void *) 50, (void *) 92);
    skiplist_put(d2, (void *) 49, (void *) 91);
    skiplist_put(d2, (void *) 48, (void *) 90);
    skiplist_put(d2, (void *) 47, (void *) 89);
    skiplist_put(d2, (void *) 46, (void *) 88);
    skiplist_put(d2, (void *) 45, (void *) 87);
    /*  the following 3 collide: */
    skiplist_put(d2, (void *) 1, (void *) 92);
    skiplist_put(d2, (void *) 5, (void *) 91);
    skiplist_put(d2, (void *) 9, (void *) 90);

    skiplist_iterator(d, &iter);

    /*  remove every key we iterate on */
    while ((key = skiplist_iterator_next(d, &iter)))
    {
        CuAssertTrue(tc, NULL != skiplist_remove(d2, key));
        skiplist_remove(d,key);
    }

    /*  check if the skiplist is empty */
    CuAssertTrue(tc, 0 == skiplist_count(d2));
    skiplist_freeall(d);
    skiplist_freeall(d2);
}

#endif
