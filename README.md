# Sky

Sky is a small [Lisp][]-like language, written for educational purposes. It is
not intended for practical use.

[Lisp]: https://en.wikipedia.org/wiki/Lisp_(programming_language)

I'm writing [a series of blog posts][blog] about developing Sky.

[blog]: https://jbm.io/sky.html

## Prerequisites

A [POSIX][]-like environment, [GNU Make][], and a [C][] compiler that supports
[C11][] are required to build Sky. In practice, the build is currently tested
with [GCC][] 7 and [Clang][] 5 on [Fedora][] 27 (x86-64).

[POSIX]:    https://en.wikipedia.org/wiki/POSIX
[GNU Make]: https://www.gnu.org/software/make/
[C]:        https://en.wikipedia.org/wiki/C_(programming_language)
[C11]:      https://en.wikipedia.org/wiki/C11_(C_standard_revision)
[GCC]:      https://gcc.gnu.org/
[Clang]:    https://clang.llvm.org/
[Fedora]:   https://getfedora.org/

## Getting started

To build and run Sky:

```
make
./src/sky
```

## License

Sky is released under the MIT/X11 license. See the [LICENSE](LICENSE) file for
details.
