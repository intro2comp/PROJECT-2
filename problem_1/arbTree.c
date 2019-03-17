#include <sys/stat.h>
#include <fcntl.h>    // close, open

#include <unistd.h>  // read, write

#include <stdio.h>   // printf
#include <stdlib.h>  // exit
#include <string.h>  // strlen

#include <ctype.h> // isdigit()

const int BUFFER_SIZE = 256;

const char *HELP_MESSAGE =
    "Error with arguments.\n"
    "Must be formatted: "
    "%s input.txt\n";

struct tree_node
{
    pid_t pid;
    int children_no; // Number of Children
    char node_name; // Name of Node

    struct tree_node *child; // Tree node pointer to child(ren)
};

char *ReadFileContents(int f)
/* Read contents from a file descriptor *f* into a heap allocated
 * string *buffer*
 */
{
    char c;
    int buffer_index = 0;

    // Allocate BUFFER_SIZE bytes on the heap
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));

    // read returns 0 when there is no more to read
    // If the result is zero (false) break the loop
    while (read(f, &c, 1))
        buffer[buffer_index++] = c;

    buffer[buffer_index++] = '\0';

    return buffer;
}

void parse_tree_file (char *buff)
{
    char *p = buff;

    while (*p) { // While there are more characters to process...

        if ( isdigit(*p) || ( (*p=='-'||*p=='+') && isdigit(*(p+1)) )) {
            // Found a number (no. of children)
            int val = strtol(p, &p, 10); // Read number
            printf("%d\n", val);
        } else {
            // Otherwise, found a character (parent name or child(ren) name)
            p++;
        }
    }
}

void read_tree_file (const char *filename)
/* Reads data in file named *filename* to char cont for parsing.
 * If the requested file cannot be opened, then the function
 * simply returns
 */
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

    parse_tree_file(cont);
}

void print_tree (struct tree_node *root)
{

}

int main(int argc, char *argv[])
{
    // Guard for invalid input
    if (argc != 2) {
        printf(HELP_MESSAGE, argv[0]);
        return 0;
    }

    read_tree_file(argv[1]);
}
