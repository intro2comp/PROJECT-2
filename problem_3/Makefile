CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -Wpedantic


all: parallel-evaluate

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

parser.o: parser.c parser.h stack.o queue.o
	$(CC) $(CFLAGS) -c parser.c

tree.o: tree.c tree.h stack.o
	$(CC) $(CFLAGS) -c tree.c

main.o: main.c parser.o tree.o
	$(CC) $(CFLAGS) -c main.c

parallel-evaluate: main.o
	$(CC) $(CFLAGS) -o "parallel-evaluate" queue.o stack.o parser.o tree.o main.o


clean:
	rm -vf *.o parallel-evaluate

