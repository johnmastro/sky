// equal.c - Compare Sky objects for equality

#include "sky.h"

bool equal(value_t v0, value_t v1)
{
    enum type_tag tag0, tag1;

    if (v0 == v1) return true;

    tag0 = get_type_tag(v0);
    tag1 = get_type_tag(v1);

    if (tag0 != tag1) return false;

    switch(tag0) {
    case TAG_INT:
    case TAG_CHAR:
    case TAG_SYMBOL:
        // These are only equal if `v0 == v1`
        return false;
    case TAG_STRING: {
        ptrdiff_t len0 = string_length(v0);
        ptrdiff_t len1 = string_length(v1);
        const unsigned char *data0, *data1;

        if (len0 != len1) return false;

        data0 = string_data(v0);
        data1 = string_data(v1);
        return memcmp(data0, data1, len0) == 0;
    }
    case TAG_LIST: {
        value_t e0, e1;

        for (;;) {
            if (v0 == NIL && v1 == NIL)
                return true;
            if (v0 == NIL || v1 == NIL)
                return false;
            e0 = first(v0);
            e1 = first(v1);
            if (!equal(e0, e1))
                return false;
            v0 = rest(v0);
            v1 = rest(v1);
        }
    }
    default: abort();
    }
}
