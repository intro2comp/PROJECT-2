#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>  // pid_t

#include <fcntl.h>
#include <unistd.h>     // fork(), pipe()

#include <stdio.h>      // exit(), fprintf(), printf(), stderr
#include <stdlib.h>     // malloc(), free(),
#include <stdbool.h>    // bool

#include <string.h>     // strlen()
#include <ctype.h>      // isalpha(), isdigit()

#include <errno.h>

#define KRED  "\x1B[31m" // RED Text Color
#define RESET  "\x1B[0m" // DEFAULT Text Color

#define BUFFER_SIZE 256

typedef struct tree_node
{
    char name;
    int children_no;                // Number of Children
    struct tree_node **children;    // Tree node array of pointers to children
} tree_node;


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


int findChar(char c, char *arr)
/* Find character from array of names. Returns index if found.
 * Otherwise, returns -1.
 */
{
    int i = 0;

    while (arr[i] != '\0') {
        if (arr[i] == c)
            return i;
        else i++;
    }

    return -1;
}


int charToInt(char c)
/* Convert character to integer */
{
    char s[2];
    s[0] = c;
    s[1] = '\0';

    return atoi(s);
}


tree_node *parse_tree_string(char *str)
/* Parse input data and store values into defined tree structure */
{
    tree_node **nodes = malloc(BUFFER_SIZE * sizeof(tree_node*));
    char *names = malloc(BUFFER_SIZE * sizeof(char));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        nodes[i] = NULL;
        names[i] = '\0';
    }

    int node_count = 0;

    char *current_char = str;

    int child_count;

    while (*current_char != '\0') {
        tree_node *current_node;

        if (!isalpha(*current_char)) { // Parent name error handler
            fprintf(stderr, KRED "Invalid parent node name in input!\n" RESET);
            exit(EXIT_FAILURE);
        }

        // Get parent name
        int name_index = findChar(*current_char, names);

        if (name_index == -1) {
            current_node = malloc(sizeof(tree_node));
            nodes[node_count] = current_node;
            names[node_count++] = *current_char;
        } else
            current_node = nodes[name_index];

        current_node->name = *(current_char++);

        if (*current_char != 32) { // Space after parent name error handler
            fprintf(stderr, KRED "Missing space after name! Found '%c'\n" RESET, *current_char);
            exit(EXIT_FAILURE);
        }

        current_char++; // Skip space after parent name

        if (!isdigit(*current_char)) { // Children number error handler
            fprintf(stderr, KRED "Invalid children number in input!\n" RESET);
            exit(EXIT_FAILURE);
        }

        // Get child count
        child_count = charToInt(*(current_char++));
        current_node->children_no = child_count;

        // Guard for no children
        if (!child_count) {
            current_node->children = NULL;
            current_char++;
            continue;
        }

        if (*current_char != ' ') { // Space after children number error handler
            fprintf(stderr, KRED "Missing space after children number! Found '%i'\n" RESET, *current_char);
            exit(EXIT_FAILURE);
        }

        current_char++; // Skip space after children number

        current_node->children = malloc(sizeof(tree_node*) * child_count);

        // Get children names
        for (int i = 0; i < child_count; i++) {

            if (!isalpha(*current_char)) { // Child name error handler
                fprintf(stderr, KRED "Invalid child node name in input!\n" RESET);
                exit(EXIT_FAILURE);
            }

            int name_index = findChar(*current_char, names); // Search if child name already exists

            if (name_index == -1) {
                tree_node *child_node = malloc(sizeof(tree_node));
                child_node->name = *current_char;
                nodes[node_count] = child_node;
                names[node_count++] = *(current_char++);
                current_node->children[i] = child_node;
            } else {
                current_node->children[i] = nodes[name_index];
                current_char++;
            }


            if (*current_char == ' ')
                current_char++;
            else if (i == child_count - 1)
                ;
            else {
                fprintf(stderr, KRED "Missing space after child name! Found '%i'\n" RESET, *current_char);
                exit(EXIT_FAILURE);
            }
        }

        if (*current_char == '\n')
            current_char++;
        else if (*current_char == '\0')
            continue;
        else {
            fprintf(stderr, KRED "Invalid line ending! Found '%i'\n"
                "Make sure you are using LF line endings.\n" RESET, *current_char);
            exit(EXIT_FAILURE);
        }
    }

    return nodes[0];
}


void print_node(tree_node *node) {
    printf("Parent: '%c'\n", node->name);

    printf("Children:");

    for (int i = 0; i < node->children_no; i++)
        printf(" '%c'", node->children[i]->name);

    printf("\n\n");

    for (int i = 0; i < node->children_no; i++)
        print_node(node->children[i]);
}


void create_process_tree(tree_node *node) {

    printf("Node '%c' has pid '%d'\n", node->name, getpid());

    pid_t pid;

    for (int i = 0; i < node->children_no; i++) {
        tree_node *child = node->children[i];
        pid = fork();

        if (pid == 0) {
            printf("Child process '%c' with PID '%d'\n", child->name, getpid());
            create_process_tree(child);
            exit(0);
        } else if (pid > 0) {
            /* the child has been forked and we are in the parent */
        } else {
            /* could not fork child */
            fprintf(stderr, KRED "Could not fork child!\n" RESET);
            exit(EXIT_FAILURE);
        }
    }

    int status;

    while ((pid = waitpid(-1, &status, 0))) {
        if (pid == -1 && errno == ECHILD) {
            break;
        } else if (pid == -1) {
            perror("waitpid");
        } else if (WIFEXITED(status)) {
            printf("'%d' exited, status=%d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("'%d' killed by signal %d\n", pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("'%d' stopped by signal %d\n", pid, WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("'%d' continued\n", pid);
        }
    }
}


int main(int argc, char *argv[])
{
    // Guard for invalid input arguments
    if (argc != 2) {
        fprintf(stderr,
            KRED "Error with arguments.\n"
            "Must be formatted: %s input.txt\n" RESET,
            argv[0]);
        return 0;
    }

    int input_fd = open(argv[1], O_RDONLY);

    char *input_string = ReadFileContents(input_fd);

    // Parse the tree into a data structure
    tree_node *root = parse_tree_string(input_string);

    printf("\nData Structure From Input:\n"
            "========================\n");
    print_node(root);

    printf("\nProcess Tree Creation:\n"
            "========================\n");
    create_process_tree(root);
}
