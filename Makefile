CC = gcc
CFLAGS = -Wall -Wextra -g -I.

app: main.c hashtable.c
	$(CC) $(CFLAGS) -o hashtable main.c hashtable.c

test: test_main.c hashtable.c unity.c
	$(CC) $(CFLAGS) -fsanitize=address,undefined -o tests test_main.c hashtable.c unity.c
	./tests

clean:
	rm -f hashtable tests
