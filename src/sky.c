// sky.c

#include "sky.h"

static void read_and_print_1(FILE *in, FILE *out)
{
    value_t value = read_from_stream(in, NULL);
    println(out, value);
}

static void read_and_print(const char *in, const char *out)
{
    FILE *infile = in ? fopen(in, "r") : stdin;
    if (!infile)
        error("Could not open for reading: %s", in);

    FILE *outfile = out ? fopen(out, "w") : stdout;
    if (!outfile)
        error("Could not open for writing: %s", out);

    read_and_print_1(infile, outfile);
}

static void run_repl(void)
{
    value_t value;
    bool eof = false;

    fputs("Welcome to Sky. Use control-c to exit.\n", stdout);

    for (;;) {
        fprintf(stdout, "> ");
        fflush(stdout);
        value = read_from_stream(stdin, &eof);
        if (eof) break;
        println(stdout, value);
    }

    fputs("\nGoodbye\n", stdout);
}

int main(int argc, const char *argv[])
{
    if (argc == 1) {
        run_repl();
    } else if (argc == 2) {
        const char *arg = argv[1];
        read_and_print((arg[0] == '-' && arg[1] == '\0' ? NULL : arg),
                       NULL);
    } else {
        error("Invalid command line arguments");
    }

    return EXIT_SUCCESS;
}
