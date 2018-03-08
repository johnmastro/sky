// read.c - Functions for reading Sky data

#include "sky.h"

#define READ_CHAR_BUFSZ     16
#define READ_ATOM_BUFSZ    128
#define READ_STRING_BUFSZ 1024

#define TOKEQ(t, n, s) ((n) == strlen(s) && (memcmp((t), (s), (n)) == 0))

static value_t read_sexp(FILE *stream, int sentinel, int *flag);

static bool isdelimiter(int c)
{
    return (isspace(c)
            || c == '(' || c == ')'
            || c == '"' || c == ';');
}

static int read_next_char(FILE *stream)
{
    int c;

    do {
        c = getc(stream);
        if (c == ';') {
            do c = getc(stream);
            while (c != '\n' && c != EOF);
        }
    } while (isspace(c));

    return c;
}

static int read_token(FILE *stream, char *buf, int size, bool escaped)
{
    assert(size > 0);

    int pos = 0;
    int c;

    if (escaped) {
        c = getc(stream);
        if (c == EOF) {
            buf[pos] = '\0';
            return 0;
        } else {
            buf[pos++] = c;
        }
    }

    while ((c = getc(stream)) != EOF) {
        if (isdelimiter(c)) {
            ungetc(c, stream);
            break;
        }
        if (pos == size - 1) {
            ungetc(c, stream);
            buf[pos] = '\0';
            return -1;
        }
        buf[pos++] = c;
    }

    buf[pos] = '\0';
    return pos;
}

#define INTEGER_IN_RANGE(n) \
    ((n) >= MOST_NEGATIVE_INT && (n) <= MOST_POSITIVE_INT)

static intptr_t maybe_parse_integer(const char *str, int len, bool *err)
{
    int sign = 1;
    intptr_t v = 0;

    if (len == 0 || (len == 1 && (str[0] == '+' || str[0] == '-'))) {
        *err = true;
        return 0;
    }

    for (int i = 0; i < len; i++) {
        int c = str[i];
        if (i == 0 && c == '+') {
            /* Nothing */;
        } else if (i == 0 && c == '-') {
            sign = -1;
        } else if (c >= '0' && c <= '9') {
            c -= '0';
            v = v*10 + c;
        } else {
            *err = true;
            return 0;
        }
    }

    if (sign < 0)
        v = -v;

    if (!INTEGER_IN_RANGE(v))
        error("Integer overflow reading: %s", str);

    return v;
}

static intptr_t parse_hex_digits(const char *str, int len, bool *err)
{
    intptr_t v = 0;

    if (len == 0) {
        *err = true;
        return 0;
    }

    for (int i = 0; i < len; i++) {
        int c = str[i];

        if (c >= '0' && c <= '9') {
            c = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            c = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            c = c - 'a' + 10;
        } else {
            *err = true;
            return v;
        }

        v = v*16 + c;
    }

    if (!INTEGER_IN_RANGE(v)) {
        char buf[len+1];
        memcpy(buf, str, len);
        buf[len] = '\0';
        error("Integer overflow reading hex escape: %s", buf);
    }

    return v;
}

static int parse_character_name(const char *str, int len)
{
    if (len == 0)
        error("Incomplete character literal");

    if (len == 1 && isgraph(str[0]))
        return str[0];

    if (TOKEQ(str, len, "space"))
        return ' ';
    if (TOKEQ(str, len, "tab"))
        return '\t';
    if (TOKEQ(str, len, "vtab"))
        return '\v';
    if (TOKEQ(str, len, "newline"))
        return '\n';
    if (TOKEQ(str, len, "return"))
        return '\r';
    if (TOKEQ(str, len, "formfeed"))
        return '\f';
    if (TOKEQ(str, len, "backspace"))
        return '\b';

    if (len == 3 && str[0] == 'x') {
        bool err = false;
        intptr_t i = parse_hex_digits(str+1, 2, &err);
        if (!err) {
            assert(i >= 0 && i <= (intptr_t)INT_MAX);
            return (int)i;
        }
    }

    error("Invalid character literal: #\\%s", str);
}

static value_t read_character(FILE *stream)
{
    char buf[READ_CHAR_BUFSZ];
    int len, c;

    len = read_token(stream, buf, READ_CHAR_BUFSZ, true);
    if (len < 0)
        error("Character literal token too long: #\\%s...", buf);

    c = parse_character_name(buf, len);
    return make_character(c);
}

static value_t read_atom(FILE *stream)
{
    char buf[READ_ATOM_BUFSZ];
    int len = read_token(stream, buf, READ_ATOM_BUFSZ, false);

    if (len < 0)
        error("Token too long");

    if (TOKEQ(buf, len, "nil"))
        return NIL;

    bool err = false;
    intptr_t i = maybe_parse_integer(buf, len, &err);

    if (!err) return make_integer(i);

    return make_symbol(make_string(buf, len));
}

static value_t read_string(FILE *stream)
{
    char buf[READ_STRING_BUFSZ];
    char esc[3];
    int pos = 0;
    int c;

    while ((c = getc(stream)) != '"') {
        if (c == EOF)
            goto eof_error;
        if (pos == READ_STRING_BUFSZ - 1)
            error("String literal too long");

        if (c == '\\') {
            c = getc(stream);
            switch(c) {
            case EOF:  goto eof_error;
            case '\n': continue;
            case 'b':  c = '\b'; break;
            case 't':  c = '\t'; break;
            case 'n':  c = '\n'; break;
            case 'v':  c = '\v'; break;
            case 'f':  c = '\f'; break;
            case 'r':  c = '\r'; break;
            case 'x': {
                bool err = false;
                if ((esc[0] = getc(stream)) == EOF)
                    goto eof_error;
                if ((esc[1] = getc(stream)) == EOF)
                    goto eof_error;
                esc[2] = '\0';
                c = parse_hex_digits(esc, 2, &err);
                if (err) goto esc_error;
                break;
            }
            default: break;
            }
        }
        buf[pos++] = c;
    }

    buf[pos] = '\0';
    return make_string(buf, pos);

eof_error:
    error("EOF while reading");
esc_error:
    error("Invalid escape sequence: \\%s", esc);
}

#define REST(x) (((struct object *)(x))->u.list.rest)

static value_t read_list(FILE *stream)
{
    value_t list = NIL;
    value_t last = NIL;

    for (;;) {
        int flag;
        value_t value = read_sexp(stream, ')', &flag);
        value_t cell;

        if (flag == ')')
            break;
        if (flag == EOF)
            error("EOF while reading");

        cell = cons(value, NIL);

        if (list == NIL)
            last = list = cell;
        else
            last = REST(last) = cell;
    }

    return list;
}

static value_t read_sharp(FILE *stream)
{
    int c = getc(stream);
    if (c == '\\')
        return read_character(stream);

    ungetc(c, stream);

    char buf[READ_ATOM_BUFSZ];
    int len = read_token(stream, buf, READ_ATOM_BUFSZ, false);

    if (len < 0)
        error("Token too long");

    if (TOKEQ(buf, len, "symbol")) {
        int flag;
        value_t sexp = read_sexp(stream, 0, &flag);
        if (flag == EOF)
            error("EOF while reading");
        if (get_type_tag(sexp) == TAG_STRING)
            return make_symbol(sexp);
        error("Expression following #symbol must be a string");
    }

    error("Invalid read syntax: #%s", buf);
}

static value_t read_sexp(FILE *stream, int sentinel, int *flag)
{
    int c = read_next_char(stream);

    if (sentinel != 0 && c == sentinel) {
        *flag = c;
        return NIL;
    }

    *flag = 0;

    switch(c) {
    case '(': return read_list(stream);
    case '"': return read_string(stream);
    case '#': return read_sharp(stream);
    case ')': error("Unmatched delimiter while reading: %c", c);
    case EOF:
        *flag = c;
        return NIL;
    default:
        ungetc(c, stream);
        return read_atom(stream);
    }
}

value_t read_from_stream(FILE *stream, bool *eof)
{
    int flag;
    value_t value = read_sexp(stream, 0, &flag);

    if (flag == EOF && eof != NULL)
        *eof = true;

    return value;
}

value_t read_from_file(const char *filename, bool *eof)
{
    value_t value;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        error("Unable to open for reading: %s", filename);
    value = read_from_stream(file, eof);
    fclose(file);
    return value;
}
