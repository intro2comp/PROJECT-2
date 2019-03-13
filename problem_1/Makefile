CC = gcc
CFLAGS = -std=c99 -g -Wall

arbTree:
	$(CC) $(CFLAGS) -o tree arbTree.c

all: tree

test: tree
	./tree input.txt

clean: clean-test
	rm -vf tree

clean-test:
	rm -vf *.out