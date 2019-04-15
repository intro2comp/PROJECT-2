CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -Wpedantic

sigs: sigs.c
	$(CC) $(CFLAGS) -o sigs sigs.c

all: sigs

test:
	./sigs input2.txt

clean: clean-test
	rm -vf sigs

clean-test:
	rm -vf *.out