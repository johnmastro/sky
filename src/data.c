// data.c - Basic data constructors and accessors

#include "sky.h"

enum type_tag get_type_tag(value_t value)
{
    if (value == NIL) return TAG_LIST;
    return ((struct object *)value)->tag;
}

struct object *make_object(enum type_tag tag)
{
    struct object *obj = xmalloc(sizeof(*obj));
    obj->tag = tag;
    return obj;
}

value_t make_integer(intptr_t value)
{
    struct object *obj = make_object(TAG_INT);
    obj->u.i = value;
    return (value_t)obj;
}

intptr_t integer_data(value_t value)
{
    return ((struct object *)value)->u.i;
}

value_t make_character(int value)
{
    struct object *obj = make_object(TAG_CHAR);
    obj->u.c = value;
    return (value_t)obj;
}

int character_data(value_t value)
{
    return ((struct object *)value)->u.c;
}

value_t make_string(const char *data, ptrdiff_t len)
{
    assert(len >= 0);
    assert(len != PTRDIFF_MAX);
    struct object *obj = make_object(TAG_STRING);
    obj->u.string.len = len;
    obj->u.string.data = xmalloc(len + 1);
    memcpy(obj->u.string.data, data, len);
    obj->u.string.data[len] = '\0';
    return (value_t)obj;
}

ptrdiff_t string_length(value_t value)
{
    return ((struct object *)value)->u.string.len;
}

const unsigned char *string_data(value_t value)
{
    return ((struct object *)value)->u.string.data;
}

int string_ref(value_t value, ptrdiff_t i)
{
    const unsigned char *data = string_data(value);
    return data[i];
}

value_t make_symbol(value_t name)
{
    assert(get_type_tag(name) == TAG_STRING);
    struct object *obj = make_object(TAG_SYMBOL);
    obj->u.symbol.name = name;
    return (value_t)obj;
}

value_t make_symbol_from_c_string(const char *data)
{
    size_t len = strlen(data);
    assert(len < (size_t)PTRDIFF_MAX);
    value_t name = make_string(data, len);
    return make_symbol(name);
}

value_t symbol_name(value_t value)
{
    return ((struct object *)value)->u.symbol.name;
}

value_t cons(value_t first, value_t rest)
{
    assert(get_type_tag(rest) == TAG_LIST);
    struct object *obj = make_object(TAG_LIST);
    obj->u.list.first = first;
    obj->u.list.rest = rest;
    return (value_t)obj;
}

value_t first(value_t value)
{
    return ((struct object *)value)->u.list.first;
}

value_t rest(value_t value)
{
    return ((struct object *)value)->u.list.rest;
}
