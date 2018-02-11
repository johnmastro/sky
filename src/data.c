// data.c - Basic data constructors and accessors

#include "sky.h"

enum type_tag get_type_tag(struct object *obj)
{
    if (obj == NIL) return TAG_LIST;
    return obj->tag;
}

struct object *make_object(enum type_tag tag)
{
    struct object *obj = xmalloc(sizeof(*obj));
    obj->tag = tag;
    return obj;
}

struct object *make_integer(intptr_t value)
{
    struct object *obj = make_object(TAG_INT);
    obj->u.i = value;
    return obj;
}

intptr_t integer_data(struct object *obj)
{
    return obj->u.i;
}

struct object *make_character(int value)
{
    struct object *obj = make_object(TAG_CHAR);
    obj->u.c = value;
    return obj;
}

int character_data(struct object *obj)
{
    return obj->u.c;
}

struct object *make_string(const char *data, ptrdiff_t len)
{
    assert(len >= 0);
    assert(len != PTRDIFF_MAX);
    struct object *obj = make_object(TAG_STRING);
    obj->u.string.len = len;
    obj->u.string.data = xmalloc(len + 1);
    memcpy(obj->u.string.data, data, len);
    obj->u.string.data[len] = '\0';
    return obj;
}

ptrdiff_t string_length(struct object *obj)
{
    return obj->u.string.len;
}

const unsigned char *string_data(struct object *obj)
{
    return obj->u.string.data;
}

int string_ref(struct object *obj, ptrdiff_t i)
{
    const unsigned char *data = string_data(obj);
    return data[i];
}

struct object *make_symbol(struct object *name)
{
    assert(get_type_tag(name) == TAG_STRING);
    struct object *obj = make_object(TAG_SYMBOL);
    obj->u.symbol.name = name;
    return obj;
}

struct object *make_symbol_from_c_string(const char *data)
{
    size_t len = strlen(data);
    assert(len < (size_t)PTRDIFF_MAX);
    struct object *name = make_string(data, len);
    return make_symbol(name);
}

struct object *symbol_name(struct object *obj)
{
    return obj->u.symbol.name;
}

struct object *cons(struct object *first, struct object *rest)
{
    assert(get_type_tag(rest) == TAG_LIST);
    struct object *obj = make_object(TAG_LIST);
    obj->u.list.first = first;
    obj->u.list.rest = rest;
    return obj;
}

struct object *first(struct object *obj)
{
    return obj->u.list.first;
}

struct object *rest(struct object *obj)
{
    return obj->u.list.rest;
}
