#include <sys/stat.h>
#include <fcntl.h>    // close, open

#include <unistd.h>  // read, write

#include <stdio.h>   // printf
#include <stdlib.h>  // exit
#include <string.h>  // strlen

#include <ctype.h> // isdigit(), isspace()
#include <stdbool.h> // bool

#include "parser.h" // for parse_tree_file()

const int BUFFER_SIZE = 256;
const char *HELP_MESSAGE =
    "Error with arguments.\n"
    "Must be formatted: "
    "%s input.txt\n";

int token_count;

struct tree_node
{
    char name; // Name of Node
    int children_no; // Number of Children
    pid_t pid; // Process ID
    struct tree_node **child; // Tree node array of pointers to child(ren)
};

void print_tokens(Token **tokens)
{
    Token *token = tokens[0];

    printf("====================\n"
            "TOKENS CREATED:\n"
            "--------------------\n");

    for(int i = 0; i < token_count; i++) {

        token = tokens[i];

        switch (token->type) {
        case PARENT:
            printf("\nPARENT: %c\n", token->name);
            break;
        case COUNT:
            printf("COUNT:  %d\n", token->value);
            break;
        case CHILD:
            printf("CHILD:  %c\n", token->name);
            break;
        default:
            break;
        }
    }

    printf("\n");
}

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

Token **parse_tree_file(char *str)
/* Takes character array and stores into tokens for tree building.
 * Includes formatting error detection for incorrect input files.
 */
{
    Token **tokens = malloc(BUFFER_SIZE * sizeof(Token*));

    int length = strlen(str);
    bool firstLetter = true;

    for (int i = 0; i < BUFFER_SIZE; i++)
        tokens[i] = malloc(sizeof(Token));

    token_count = 0;
    int value = -1;

    for (int i = 0; i < length; i++) {

        if(str[i] == ' ' || str[i] == '\n') {} //Skip spaces or newlines

        else if(firstLetter) { // Get Parent Node Name

            if(isdigit(str[i]) || isalpha(str[i+1])) {
                printf("Input file formatting error. PARENT must have ONE CHAR name.\n\n");
                exit(EXIT_FAILURE);
            }

            tokens[token_count]->type = PARENT;
            tokens[token_count++]->name = str[i];
            firstLetter = false;
        }

        else if(isdigit(str[i])) { // Get Number of Child Processes

            if(isdigit(str[i+1]) || value != -1) {
                printf("Input file formatting error. NUMBER of child processes limited from 0-9.\n\n");
                exit(EXIT_FAILURE);
            }

            value = str[i] - '0';

            tokens[token_count]->type = COUNT;
            tokens[token_count++]->value = value;
        }

        else if(!firstLetter) { // Get Child Process Names

            while (value) {

                if(str[i] == ' ' || str[i] == '\n') {} //Skip spaces or newlines

                else {

                    if(isdigit(str[i]) || isalpha(str[i+1])) {
                        printf("Input file formatting error. CHILD must have ONE CHAR name.\n\n");
                        exit(EXIT_FAILURE);
                    }

                    tokens[token_count]->type = CHILD;
                    tokens[token_count++]->name = str[i];
                    value--;
                }

                i++;
            }

            if (str[i] == ' ') {
                printf("Input file formatting error. Number of children exceeded input.\n\n");
                exit(EXIT_FAILURE);
            }

            value = -1;
            firstLetter = true;
        }
    }

    return tokens;
}

char *read_tree_file(const char *filename)
/* Reads data in file named *filename* to char cont for parsing.
 * If the requested file cannot be opened, then the function
 * simply returns
 */
{
    int fdin;

    fdin = open(filename, O_RDONLY);

    if (fdin == -1) {
        printf("Could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char *cont;
    cont = ReadFileContents(fdin);

    close(fdin);

    printf("\n====================\n"
            "GIVEN INPUT FILE:\n"
            "--------------------\n\n");
    printf("%s\n", cont);

    return cont;
}

void create_tree()
{
}

void print_tree()
{
    printf("====================\n"
            "TREE CREATED:\n"
            "--------------------\n");
}

int main(int argc, char *argv[])
{
    // Guard for invalid input arguments
    if (argc != 2) {
        printf(HELP_MESSAGE, argv[0]);
        return 0;
    }

    Token **tokens = parse_tree_file(read_tree_file(argv[1]));

    print_tokens(tokens);
}
