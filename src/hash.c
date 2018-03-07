// hash.c - Compute hash codes

#include "sky.h"

#define HASH_PRIME 31

static uintptr_t hash_string(value_t string)
{
    ptrdiff_t len = string_length(string);

    if (len == 0) return 0;

    const unsigned char *data = string_data(string);
    uintptr_t result = 1;

    for (ptrdiff_t i = 0; i < len; i++)
        result = result*HASH_PRIME + data[i];

    return result;
}

static uintptr_t hash_list(value_t list)
{
    if (list == NIL) return 0;

    uintptr_t result = 1;

    while (list != NIL) {
        value_t elt = first(list);
        result = result*HASH_PRIME + hash(elt);
        list = rest(list);
    }

    return result;
}

uintptr_t hash(value_t value)
{
    enum type_tag tag = get_type_tag(value);

    switch(tag) {
    case TAG_INT:    return integer_data(value);
    case TAG_CHAR:   return character_data(value);
    case TAG_SYMBOL: return value;
    case TAG_STRING: return hash_string(value);
    case TAG_LIST:   return hash_list(value);
    default:         abort();
    }
}
