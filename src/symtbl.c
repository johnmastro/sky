// symtbl.c - Symbol table

#include "sky.h"

#define SYMTBL_LOAD_FACTOR  0.5
#define SYMTBL_INITIAL_SIZE 256

#define SYMTBL_SIZE(x)  ((x)->size)
#define SYMTBL_COUNT(x) ((x)->count)
#define SYMTBL_KEYS(x)  ((x)->keys)
#define SYMTBL_VALS(x)  ((x)->vals)

struct symtbl *make_symtbl(void)
{
    size_t size = SYMTBL_INITIAL_SIZE;
    value_t *keys = xcalloc(size, sizeof(*keys));
    value_t *vals = xcalloc(size, sizeof(*vals));
    struct symtbl *table = xmalloc(sizeof(*table));
    table->size = size;
    table->count = 0;
    table->keys = keys;
    table->vals = vals;
    return table;
}

static size_t findslot(value_t *keys, size_t size, value_t key, bool *found)
{
    assert(key != NIL);
    size_t i = hash(key) % size;
    while (keys[i] != NIL && !equal(key, keys[i]))
        i = (i + 1) % size;
    if (found != NULL)
        *found = keys[i] != NIL;
    return i;
}

static bool setval(value_t *keys, value_t *vals, size_t size,
                   value_t key, value_t val)
{
    bool found;
    size_t i = findslot(keys, size, key, &found);
    keys[i] = key;
    vals[i] = val;
    return found;
}

static value_t getval(value_t *keys, value_t *vals, size_t size, value_t key)
{
    size_t i = findslot(keys, size, key, NULL);
    return vals[i];
}

void symtbl_resize(struct symtbl *table, size_t size)
{
    assert(size > SYMTBL_SIZE(table));
    assert(size % 2 == 0);

    value_t *oldkeys = SYMTBL_KEYS(table);
    value_t *oldvals = SYMTBL_VALS(table);
    value_t *keys = xcalloc(size, sizeof(*keys));
    value_t *vals = xcalloc(size, sizeof(*vals));
    size_t oldsize = SYMTBL_SIZE(table);

    for (size_t i = 0; i < oldsize; i++) {
        value_t k = oldkeys[i];
        value_t v = oldvals[i];
        if (k == NIL) continue;
        size_t j = findslot(keys, size, k, NULL);
        keys[j] = k;
        vals[j] = v;
    }

    SYMTBL_KEYS(table) = keys;
    SYMTBL_VALS(table) = vals;
    SYMTBL_SIZE(table) = size;

    free(oldkeys);
    free(oldvals);
}

void symtbl_set(struct symtbl *table, value_t key, value_t val)
{
    size_t size = SYMTBL_SIZE(table);
    size_t count = SYMTBL_COUNT(table);

    if (count / (double)size >= SYMTBL_LOAD_FACTOR) {
        size *= 2;
        symtbl_resize(table, size);
    }

    value_t *keys = SYMTBL_KEYS(table);
    value_t *vals = SYMTBL_VALS(table);

    if (!setval(keys, vals, size, key, val))
        SYMTBL_COUNT(table)++;
}

value_t symtbl_ref(struct symtbl *table, value_t key)
{
    value_t *keys = SYMTBL_KEYS(table);
    value_t *vals = SYMTBL_VALS(table);
    size_t size = SYMTBL_SIZE(table);
    return getval(keys, vals, size, key);
}

bool symtbl_del(struct symtbl *table, value_t key)
{
    value_t *keys = SYMTBL_KEYS(table);
    value_t *vals = SYMTBL_VALS(table);
    size_t size = SYMTBL_SIZE(table);
    size_t i = findslot(keys, size, key, NULL);

    if (keys[i] == NIL) return false;

    keys[i] = NIL;
    vals[i] = NIL;

    for (i = (i+1) % size; keys[i] != NIL; i = (i+1) % size) {
        value_t k = keys[i];
        value_t v = vals[i];
        keys[i] = NIL;
        vals[i] = NIL;
        setval(keys, vals, size, k, v);
    }

    SYMTBL_COUNT(table)--;
    return true;
}
