// sky.h

#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdnoreturn.h>

typedef intptr_t value_t;

/*
 * Pointer tags
 *
 * If the two least significant bits of a `value_t` are non-zero, they represent
 * the type tag of an immediate value. The value is obtained by shifting the
 * `value_t` two places right to remove the tag.
 *
 * Otherwise, the `value_t` is a pointer to a heap-allocated `struct object`.
 *
 * | Binary | Decimal | Type      |
 * |--------+---------+-----------|
 * |     00 |       0 | Pointer   |
 * |     01 |       1 | Integer   |
 * |     10 |       2 | Character |
 * |     11 |       3 | Unused    |
 *
 */

#define TAG_BITS 2
#define PTR_BITS (sizeof(intptr_t) * CHAR_BIT)
#define VAL_BITS (PTR_BITS - TAG_BITS)
#define VAL_MASK (-(1 << TAG_BITS))

enum type_tag {
    TAG_NONE = 0,
    TAG_INT,
    TAG_CHAR,
    TAG_UNUSED,
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
        struct string string;
        struct symbol symbol;
        struct list list;
    } u;
};

#define NIL ((value_t)0)

#define MOST_POSITIVE_INT (INTPTR_MAX >> TAG_BITS)
#define MOST_NEGATIVE_INT (-1 - MOST_POSITIVE_INT)

// Defined in data.c
enum type_tag get_type_tag(value_t value);
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

// Defined in read.c
value_t read_from_stream(FILE *stream, bool *eof);
value_t read_from_file(const char *filename, bool *eof);

// Defined in print.c
void print(FILE *stream, value_t value);
void println(FILE *stream, value_t value);
