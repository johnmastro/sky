// print.c - Functions for printing Sky data

#include "sky.h"

static void print_integer(FILE *stream, value_t value)
{
    intptr_t i = integer_data(value);
    fprintf(stream, "%" PRIdPTR, i);
}

static void print_character(FILE *stream, value_t value)
{
    int c = character_data(value);

    fputs("#\\", stream);

    if (isgraph(c)) {
        putc(c, stream);
        return;
    }

    switch(c) {
    case '\b': fputs("backspace", stream); return;
    case '\t': fputs("tab", stream); return;
    case '\n': fputs("newline", stream); return;
    case '\v': fputs("vtab", stream); return;
    case '\f': fputs("formfeed", stream); return;
    case '\r': fputs("return", stream); return;
    case ' ':  fputs("space", stream); return;
    default:   fprintf(stream, "x%02X", c); return;
    }
}

static bool quotename(value_t name, ptrdiff_t len)
{
    bool intlike = true;

    if (len == 0) return true;

    for (ptrdiff_t i = 0; i < len; i++) {
        int c = string_ref(name, i);
        if (c < '!'     // Control and whitespace characters
            || c > '~'  // DEL
            || c == '(' || c == ')'
            || c == '"'
            || c == ';'
            || (i == 0 && c == '#'))
            return true;
        if (intlike
            && !((i == 0 && (c == '-' || c == '+'))
                 || (c >= '0' && c <= '9')))
            intlike = false;
    }

    return intlike;
}

static void print_string_1(FILE *stream, value_t value, bool symbol)
{
    ptrdiff_t len = string_length(value);

    if (symbol && quotename(value, len)) {
        fputs("#symbol ", stream);
        symbol = false;
    }

    if (!symbol) putc('"', stream);

    for (ptrdiff_t i = 0; i < len; i++) {
        int c = string_ref(value, i);

        switch(c) {
        case '\b': fputs("\\b", stream); break;
        case '\t': fputs("\\t", stream); break;
        case '\n': fputs("\\n", stream); break;
        case '\v': fputs("\\v", stream); break;
        case '\f': fputs("\\f", stream); break;
        case '\r': fputs("\\r", stream); break;
        case '"':  fputs("\\\"", stream); break;
        case '\\': fputs("\\\\", stream); break;
        default:
            if (isprint(c))
                putc(c, stream);
            else
                fprintf(stream, "\\x%02X", c);
            break;
        }
    }

    if (!symbol) putc('"', stream);
}

static void print_string(FILE *stream, value_t value)
{
    print_string_1(stream, value, false);
}

static void print_symbol(FILE *stream, value_t value)
{
    value_t name = symbol_name(value);
    print_string_1(stream, name, true);
}

static void print_list(FILE *stream, value_t value)
{
    putc('(', stream);

    while (value != NIL) {
        value_t fst = first(value);
        print(stream, fst);
        value = rest(value);
        if (value != NIL)
            putc(' ', stream);
    }

    putc(')', stream);
}

void print(FILE *stream, value_t value)
{
    enum type_tag tag = get_type_tag(value);

    switch(tag) {
    case TAG_INT:    print_integer(stream, value); return;
    case TAG_CHAR:   print_character(stream, value); return;
    case TAG_STRING: print_string(stream, value); return;
    case TAG_SYMBOL: print_symbol(stream, value); return;
    case TAG_LIST:   print_list(stream, value); return;
    default:         abort();
    }
}

void println(FILE *stream, value_t value)
{
    print(stream, value);
    putc('\n', stream);
}
