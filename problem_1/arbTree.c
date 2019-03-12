#include <stdio.h>
#include <stdlib.h>

struct tree_node
{
    int children_no;
}

void read_tree_file (const char *filename)
{

}

char *ReadFileContents(int f)
{
    char c;
    int buffer_index = 0;

    char *buffer = malloc(BUFFER_SIZE * sizeof(char));

    while (read(f, &c, 1))
        buffer[buffer_index++] = c;

    buffer[buffer_index++] = '\0';

    return buffer;
}

void print_tree (struct tree_node *root)
{

}

int main(char argc, char *argv[]){

}
