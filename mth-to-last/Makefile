CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g

all: mth-to-last

stack: mth-to-last.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f mth-to-last
