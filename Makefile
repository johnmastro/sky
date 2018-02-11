NAME   = sky
SHELL  = /bin/sh
CC     = cc
CFLAGS = -std=c11 -O0 -ggdb3 -Wall -Wextra -ftrapv -fno-strict-aliasing
LDLIBS =

ifeq ($(findstring clang, $(CC)), clang)
	LDFLAGS = --rtlib=compiler-rt
else
	LDFLAGS =
endif

headers := $(wildcard src/*.h)
sources := $(wildcard src/*.c)
objects := $(patsubst %.c,%.o,$(sources))

all: $(NAME)

%.o: %.c $(headers)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(objects)
	$(CC) $(LDFLAGS) -o src/$@ $(objects) $(LDLIBS)

clean:
	$(RM) src/*.o src/$(NAME)

tags:
	etags $(sources) $(headers)

TAGS: tags

.PHONY: clean tags TAGS
