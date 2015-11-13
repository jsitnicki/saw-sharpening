CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g $(shell pkg-config --cflags cmocka)
LDFLAGS = $(shell pkg-config --libs cmocka)

all: tail-ptr

stack: tail-ptr.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f tail-ptr
