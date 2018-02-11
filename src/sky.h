// sky.h

#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <stdnoreturn.h>

enum type_tag {
    TAG_NONE = 0,
    TAG_INT,
    TAG_CHAR,
    TAG_STRING,
    TAG_SYMBOL,
    TAG_LIST
};

struct string {
    ptrdiff_t len;
    unsigned char *data;
};

struct symbol {
    struct object *name;
};

struct list {
    struct object *first;
    struct object *rest;
};

struct object {
    enum type_tag tag;

    union {
        intptr_t i;
        int c;
        struct string string;
        struct symbol symbol;
        struct list list;
    } u;
};

#define NIL ((struct object *)0)

// Defined in data.c
enum type_tag get_type_tag(struct object *obj);
struct object *make_object(enum type_tag tag);
struct object *make_integer(intptr_t value);
intptr_t integer_data(struct object *obj);
struct object *make_character(int value);
int character_data(struct object *obj);
struct object *make_string(const char *data, ptrdiff_t len);
ptrdiff_t string_length(struct object *obj);
const unsigned char *string_data(struct object *obj);
int string_ref(struct object *obj, ptrdiff_t i);
struct object *make_symbol(struct object *name);
struct object *make_symbol_from_c_string(const char *data);
struct object *symbol_name(struct object *obj);
struct object *cons(struct object *first, struct object *rest);
struct object *first(struct object *obj);
struct object *rest(struct object *obj);

// Defined in error.c
noreturn void error(const char *format, ...);

// Defined in memory.c
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *ptr, size_t size);
