#include <sys/stat.h>
#include <fcntl.h>    // close, open

#include <unistd.h>  // read, write

#include <stdio.h>   // printf
#include <stdlib.h>  // exit
#include <string.h>  // strlen


const int BUFFER_SIZE = 256;

struct tree_node
{
    pid_t pid;
    int children_no;
};

const char *HELP_MESSAGE =
    "%s tree_input_file.txt file_2\n";

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

void read_tree_file (const char *filename)
{
    int fdin;

    fdin = open(filename, O_RDONLY);

    if (fdin == -1) {
        printf("Could not open file '%s'\n", filename);
        return;
    }

    char *cont;

    cont = ReadFileContents(fdin);

    close(fdin);

    printf("%s\n", cont);

}

void print_tree (struct tree_node *root)
{

}

int main(char argc, char *argv[])
{

    if (argc < 1) {
        printf(HELP_MESSAGE, argv[0]);
        return 0;
    }

    read_tree_file(argv[1]);
}
