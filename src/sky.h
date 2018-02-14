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

typedef intptr_t value_t;

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
    value_t name;
};

struct list {
    value_t first;
    value_t rest;
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

#define NIL ((value_t)0)

// Defined in data.c
enum type_tag get_type_tag(value_t value);
struct object *make_object(enum type_tag tag);
value_t make_integer(intptr_t value);
intptr_t integer_data(value_t value);
value_t make_character(int value);
int character_data(value_t value);
value_t make_string(const char *data, ptrdiff_t len);
ptrdiff_t string_length(value_t value);
const unsigned char *string_data(value_t value);
int string_ref(value_t value, ptrdiff_t i);
value_t make_symbol(value_t name);
value_t make_symbol_from_c_string(const char *data);
value_t symbol_name(value_t value);
value_t cons(value_t first, value_t rest);
value_t first(value_t value);
value_t rest(value_t value);

// Defined in error.c
noreturn void error(const char *format, ...);

// Defined in memory.c
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *ptr, size_t size);
