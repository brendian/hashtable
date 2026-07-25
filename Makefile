CC = gcc
CFLAGS = -Wall -Wextra -g -I.
ASANFLAGS = -fsanitize=address,undefined

all: app test

app: main.c hashtable.c
	$(CC) $(CFLAGS) $(ASANFLAGS) -o hashtable main.c hashtable.c

test: test_main.c hashtable.c unity.c
	$(CC) $(CFLAGS) $(ASANFLAGS) -o tests test_main.c hashtable.c unity.c
	./tests

clean:
	rm -f hashtable tests

.PHONY: all app test clean
