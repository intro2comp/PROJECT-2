#include <sys/stat.h>
#include <fcntl.h>    // close, open

#include <unistd.h>  // read, write

#include <stdio.h>   // printf
#include <stdlib.h>  // exit
#include <string.h>  // strlen

#include <ctype.h> // isdigit(), isspace()
#include <stdbool.h> // bool

#include "parser.h" // for parse_tree_file()

#define EXIT_FAILURE 1

const int BUFFER_SIZE = 256;

const char *HELP_MESSAGE =
    "Error with arguments.\n"
    "Must be formatted: "
    "%s input.txt\n";

struct tree_node
{
    pid_t pid;
    int children_no; // Number of Children
    char name; // Name of Node

    struct tree_node **child; // Tree node array of pointers to child(ren)
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

void print_tokens (Token **tokens, int count)
{
    for (int i = 0; i < count; i++) {
        printf("Token: %c   Type: %d\n", tokens[i]->value, tokens[i]->type);
    }
}

void parse_tree_file (char *str)
{
    Token **tokens = malloc(BUFFER_SIZE * sizeof(Token*));

    int length = strlen(str);
    bool firstLetter = true;

    for (int i = 0; i < BUFFER_SIZE; i++)
        tokens[i] = malloc(sizeof(Token));

    int token_count = 0;

    for (int i = 0; i < length; i++) {

        if(firstLetter) { // Get Parent Node Name
            tokens[token_count]->type = PARENT;
            tokens[token_count++]->value = str[i];
            firstLetter = false;
        }

        else if(isdigit(str[i])) { // Get Number of Child Processes
            int value = str[i] - 48;

            tokens[token_count]->type = COUNT;
            tokens[token_count++]->value = value;
        }

        else if(!firstLetter) { // Get Child Process Names
            for (int j = i; j < j + tokens[token_count-1]->value; j++) {

                if (str[j] != ' '){
                    tokens[token_count]->type = CHILD;
                    tokens[token_count++]->value = str[j];
                }
                i++;
            }

            firstLetter = true;
        }

        else return;
    }

    print_tokens (tokens, token_count);
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
