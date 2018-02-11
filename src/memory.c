// memory.c - Functions for allocating and managing memory

#include "sky.h"

void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (!p)
        error("Memory full [requested %zu byte(s)]", size);
    return p;
}

void *xcalloc(size_t count, size_t size)
{
    void *p = calloc(count, size);
    if (!p)
        error("Memory full [requested %zu element(s) of %zu byte(s)]",
              count, size);
    return p;
}

void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (!p)
        error("Memory full [requested %zu byte(s)]", size);
    return p;
}
