#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "skiplist.h"

static long __ulong_compare(
    const void *e1,
    const void *e2,
    const void* udata __attribute__((unused)))
{
    const long i1 = (unsigned long) e1, i2 = (unsigned long) e2;

//      return !(*i1 == *i2); 
    return i1 - i2;
}

void TestSkiplist_new(CuTest * tc)
{
    skiplist_t *d;

    d = skiplist_new(__ulong_compare, NULL);

    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplist_Put(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplist_PutEnsuresCapacity(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 51, (void *) 92);

    CuAssertTrue(tc, 2 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplist_PutHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
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

void Testskiplist_GetHandlesCollisionByTraversingChain(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 5, (void *) 93);

    CuAssertTrue(tc, 2 == skiplist_count(d));

    val = (unsigned long) skiplist_get(d, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);
    skiplist_freeall(d);
}

void Testskiplist_RemoveReturnsNullIfMissingAndTraversesChain(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 1, (void *) 92);

    val = (unsigned long) skiplist_remove(d, (void *) 5);
    CuAssertTrue(tc, 0 == val);

    val = (unsigned long) skiplist_remove(d, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);

    skiplist_freeall(d);
}

void Testskiplist_RemoveHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;
    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
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

#if 0
void T_estskiplist_PutEntry(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_entry_t entry;

    d = skiplist_new(__ulong_compare, NULL);
    entry.k = (void *) 50;
    entry.v = (void *) 92;
    skiplist_put_entry(d, &entry);
    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}
#endif

void Testskiplist_Get(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    val = (unsigned long) skiplist_get(d, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    skiplist_freeall(d);
}

void Testskiplist_ContainsKey(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__ulong_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == skiplist_contains_key(d, (void*) 50));
    skiplist_freeall(d);
}


void Testskiplist_DoublePut(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 50, (void *) 92);
    skiplist_put(d, (void *) 50, (void *) 23);
    val = (unsigned long) skiplist_get(d, (void *) 50);
    CuAssertTrue(tc, val == 23);
    CuAssertTrue(tc, 1 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplist_PutAtHead(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 45, (void *) 87);
    skiplist_put(d, (void *) 46, (void *) 88);
    skiplist_put(d, (void *) 47, (void *) 89);
    skiplist_put(d, (void *) 48, (void *) 90);
    skiplist_put(d, (void *) 49, (void *) 91);
    skiplist_put(d, (void *) 50, (void *) 92);
    val = (unsigned long) skiplist_get(d, (void *) 48);
    CuAssertTrue(tc, val == 90);
    skiplist_freeall(d);
}

void Testskiplist_Get2(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);
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

void Testskiplist_Remove(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);

    skiplist_put(d, (void *) 50, (void *) 92);

    val = (unsigned long) skiplist_remove(d, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

void Testskiplist_ClearRemovesAll(
    CuTest * tc
)
{
    skiplist_t *d;

    unsigned long val;

    d = skiplist_new(__ulong_compare, NULL);

    skiplist_put(d, (void *) 1, (void *) 92);
    skiplist_put(d, (void *) 2, (void *) 102);
    skiplist_clear(d);

    val = (unsigned long) skiplist_get(d, (void *) 1);
    CuAssertTrue(tc, 0 == val);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

/* The clear function will need to clean the whole chain */
void Testskiplist_ClearHandlesCollision(
    CuTest * tc
)
{
    skiplist_t *d;

    d = skiplist_new(__ulong_compare, NULL);

    skiplist_put(d, (void *) 1, (void *) 50);
    /* all of the rest cause collisions */
    skiplist_put(d, (void *) 5, (void *) 51);
    skiplist_put(d, (void *) 9, (void *) 52);
    skiplist_clear(d);
    CuAssertTrue(tc, 0 == skiplist_count(d));
    skiplist_freeall(d);
}

#if 0
void T_estskiplist_DoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_iterator_t iter;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_iterator(d, &iter);
    CuAssertTrue(tc, 0 == skiplist_iterator_has_next(d, &iter));
    skiplist_freeall(d);
}

void T_estskiplist_RemoveItemDoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_iterator_t iter;

    d = skiplist_new(__ulong_compare, NULL);
    skiplist_put(d, (void *) 9, (void *) 52);
    skiplist_remove(d, (void *) 9);
    skiplist_iterator(d, &iter);
    CuAssertTrue(tc, 0 == skiplist_iterator_has_next(d, &iter));
    skiplist_freeall(d);
}

void T_estskiplist_Iterate(
    CuTest * tc
)
{
    skiplist_t *d;

    skiplist_t *d2;

    skiplist_iterator_t iter;

    void *key;

    d = skiplist_new(__ulong_compare, NULL);
    d2 = skiplist_new(__ulong_compare, NULL);

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

void T_estskiplist_IterateHandlesCollisions(
    CuTest * tc
)
{
    skiplist_t *d, *d2;

    skiplist_iterator_t iter;

    void *key;

    d = skiplist_new(__uint_hash, __ulong_compare, 4);
    d2 = skiplist_new(__uint_hash, __ulong_compare, 4);

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

void T_estskiplist_IterateAndRemoveDoesntBreakIteration(
    CuTest * tc
)
{
    skiplist_t *d;
    skiplist_t *d2;
    skiplist_iterator_t iter;
    void *key;

    d = skiplist_new(__ulong_compare, NULL);
    d2 = skiplist_new(__ulong_compare, NULL);

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
