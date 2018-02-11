// error.c - Functions for signaling errors

#include <stdarg.h>

#include "sky.h"

#define ERROR_BUFSZ 512

noreturn static void verror(const char *format, va_list ap)
{
    static char errbuf[ERROR_BUFSZ];
    int n = vsnprintf(errbuf, ERROR_BUFSZ, format, ap);
    if (n > 0)
        fprintf(stderr, "Error: %s\n", errbuf);
    abort();
}

noreturn void error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    verror(format, ap);
    // Unreachable
    va_end(ap);
}
