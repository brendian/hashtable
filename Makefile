CC = gcc
CFLAGS = -Wall -Wextra -g -I.
ASANFLAGS = -fsanitize=address,undefined

all: app test

app: src/main.c src/hashtable.c
	$(CC) $(CFLAGS) $(ASANFLAGS) -o bin/hashtable src/main.c src/hashtable.c

test: tests/test_main.c src/hashtable.c src/unity.c
	$(CC) $(CFLAGS) $(ASANFLAGS) -o bin/tests tests/test_main.c src/hashtable.c src/unity.c
	./bin/tests

clean:
	rm -f hashtable tests

.PHONY: all app test clean
