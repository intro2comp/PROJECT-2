#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>
#include <ctype.h>

#define KRED  "\x1B[31m" // RED Text Color
#define RESET  "\x1B[0m" // DEFAULT Text Color

#define BUFFER_SIZE 256

void wait_for_children();
void sigchild_handler(int signum);
void explain_wait_status(pid_t pid, int status);

typedef struct tree_node
{
    char name;
    int children_no;                // Number of Children
    pid_t pid;                      // Process ID
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
            fprintf(stderr, KRED "Invalid line ending! Found '%i'\n" RESET, *current_char);
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

    printf("Node '%c' was created and has pid '%d'\n\n", node->name, getpid());
    fflush(stdout);

    pid_t pid;
    
     for (int i = 0; i < node->children_no; i++) {
        tree_node *child = node->children[i];
        pid = fork();

        if (pid == 0) {
            create_process_tree(child);
            wait_for_children();
            //signal (SIGSTOP, sigchild_handler);
            //raise(SIGSTOP);
        }

         else if (pid > 0) {
             // printf("Child process '%c' with PID '%d' has been created\n", child->name, getpid());
             signal (SIGCONT, sigchild_handler);
             raise(SIGCONT);
            /* the child has been forked and we are in the parent */
         }

        else {
            /* could not fork child */
            fprintf(stderr, KRED "Could not fork child!\n" RESET);
            exit(EXIT_FAILURE);
        }
    }

}

    void wait_for_children(){ // Validates that all children
    int status;

        while (wait(&status) > 0)
        {
            exit(0);
        }
    }

void sigchild_handler(int signum)
{
    pid_t p;
    int status = signum;
    do
    {
        p = waitpid(-1, &status, WUNTRACED);
            
        if (p<0){
            perror("waitpid");
            exit(1);
        }
        explain_wait_status(p, status);

        if (WIFEXITED(status)||WIFSIGNALED(status)){}
            // printf("A child has died");

        if(WIFSTOPPED(status)){}
            // printf("A child has stopped due to SIG... ");
    }
    while (p>0);
}

void explain_wait_status(pid_t pid, int status)
{
    if(WIFEXITED(status))
     {
        printf ("Child with PID = %ld terminated normally, exit status = %d\n",(long)pid, WEXITSTATUS(status));
        fflush(stdout);
     }
    else if (WIFSIGNALED (status))
    {
        printf("Child with PID = %ld was terminated by a signal, signo = %d\n",(long)pid, WTERMSIG(status));
        fflush(stdout);
    }   
    else if (WIFSTOPPED(status))
    {
        printf("Child with PID = %ld has been stopped by a signal, signo = %d\n", (long)pid, WSTOPSIG(status));
        exit(17);
        fflush(stdout);
    }
    else {
        printf ("%s: Internal error: Unhandled case, PID = %ld, status = %d\n",__func__, (long) pid, status);
        fflush(stdout);
        exit(1);
    }
}
    
int main(int argc, char *argv[])
{
    // Guard for invalid input arguments
    if (argc != 2) {
        fprintf(stderr, KRED "Error with arguments.\n"
            "Must be formatted: %s input.txt\n" RESET, argv[0]);
        return 0;
    }

    int input_fd = open(argv[1], O_RDONLY);

    char *input_string = ReadFileContents(input_fd);

    tree_node *root = parse_tree_string(input_string);

    print_node(root);

    create_process_tree(root);

}