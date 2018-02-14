// sky.c

#include "sky.h"

static void check_constructors_and_accessors(void)
{
    value_t integer, character, string, symbol, list;
    ptrdiff_t i = 100;
    int c = 'a';
    const char str[] = "stuff";
    ptrdiff_t str_len = (ptrdiff_t)strlen(str);

    integer = make_integer(i);
    assert(get_type_tag(integer) == TAG_INT);
    assert(integer_data(integer) == i);

    character = make_character(c);
    assert(get_type_tag(character) == TAG_CHAR);
    assert(character_data(character) == c);

    string = make_string(str, str_len);
    assert(get_type_tag(string) == TAG_STRING);
    assert(string_length(string) == str_len);
    assert(memcmp(string_data(string), str, str_len) == 0);

    symbol = make_symbol_from_c_string(str);
    assert(get_type_tag(symbol) == TAG_SYMBOL);
    assert(memcmp(string_data(string),
                  string_data(symbol_name(symbol)),
                  str_len) == 0);

    list = cons(integer, NIL);
    list = cons(character, list);
    assert(get_type_tag(list) == TAG_LIST);
    assert(character_data(first(list)) == c);
    assert(integer_data(first(rest(list))) == i);
    assert(rest(rest(list)) == NIL);
}

int main(void)
{
    check_constructors_and_accessors();
    return EXIT_SUCCESS;
}
