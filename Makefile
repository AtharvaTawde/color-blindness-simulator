# Makefile for CSE 13S, Spring 2023, Assignment 5

CC=clang
debug:   CFLAGS=-Wall -Wextra -Wstrict-prototypes -Werror -pedantic -gdwarf-4
release: CFLAGS=-Wall -Wextra -Wstrict-prototypes -Werror -pedantic
LDFLAGS=-lm

_E1=colorb
_E2=iotest
COLORB_DEPENDENCIES= io.o bmp.o colorb.o
IO_DEPENDENCIES = io.o iotest.o

.PHONY: all debug release clean

all: debug

debug: $(_E1) $(_E2)

release: $(_E1) $(_E2)

$(_E1): $(COLORB_DEPENDENCIES)
	$(CC) $(LDFLAGS) -o $@ $^

$(_E2): $(IO_DEPENDENCIES)
	$(CC) $(LDFLAGS) -o $@ $^
	

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(_E1) $(_E2) $(COLORB_DEPENDENCIES) $(IO_DEPENDENCIES)
	rm -rf bmps/*-colorb.bmp

format:
	clang-format -i --style=file *.[ch]
