CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -Wpedantic

tree: arbTree.c
	$(CC) $(CFLAGS) -o tree arbTree.c

all: tree

test:
	./tree input.txt

clean: clean-test
	rm -vf tree

clean-test:
	rm -vf *.out