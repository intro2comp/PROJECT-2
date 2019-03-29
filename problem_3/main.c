#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "parser.h"
#include "tree.h"


/* Print a single expression token */
void print_token(Token *token)
{
  switch (token->type) {
    case NUMBER:
      printf("|%d|", token->value);
      break;
    case ADDITION:
      printf("+");
      break;
    case MULTIPLICATION:
      printf("x");
      break;
    case OPEN_BRACKET:
      printf("[");
      break;
    case OPEN_PAREN:
      printf("(");
      break;
    case CLOSE_BRACKET:
      printf("]");
      break;
    case CLOSE_PAREN:
      printf(")");
      break;
    default:
      break;
  }
}


/* Print an array of expression tokens */
void print_token_array(Token **tokens)
{
  int token_index = 0;

  Token *token = tokens[0];

  while (token) {
    print_token(token);
    token = tokens[++token_index];
  }

  printf("\n");
}


/* Print a single operation node and its depth */
void print_node(Node* node, int depth)
{
  if (!node)
    return;

  switch (node->type) {
    case VALUE:
      printf("[%d] Value: %d\n", depth, node->value.num);
      break;
    case OPERATION:
      switch (node->value.op) {
        case OP_ADD:
          printf("[%d] Operation: Add\n", depth);
          break;
        case OP_MUL:
          printf("[%d] Operation: Multiply\n", depth);
          break;
      }

      print_node(node->left, depth + 1);
      print_node(node->right, depth + 1);

      break;
  }
}


/* Print a tree of operation nodes */
void print_tree(Node* root)
{
  print_node(root, 0);
}


int evaluate_node(Node *node)
{
  int left_value, right_value;

  if (node->left->type == OPERATION)
    left_value = evaluate_node(node->left);
  else
    left_value = node->left->value.num;

  if (node->right->type == OPERATION)
    right_value = evaluate_node(node->right);
  else
    right_value = node->right->value.num;

  int result;

  if (node->value.op == OP_ADD)
    result = left_value + right_value;
  else
    result = left_value * right_value;

  return result;
}


int parallel_evaluate_node(Node *node)
{
  int pipefd[2];
  pid_t pid;

  if (pipe(pipefd) == -1) {
    perror("pipe");
    _exit(EXIT_FAILURE);
  }

  pid = fork();

  if (pid == -1) {
    perror("fork");
    _exit(EXIT_FAILURE);
  }

  if (pid == 0) { // Child process
    int left_val, right_val;
    Operation op;

    read(pipefd[0], &left_val, sizeof(int));
    read(pipefd[0], &right_val, sizeof(int));
    read(pipefd[0], &op, sizeof(Operation));

    close(pipefd[0]);

    int res;  // Where the result is stored

    if (op == OP_ADD)
      res = left_val + right_val;
    else
      res = left_val * right_val;

    write(pipefd[1], &res, sizeof(int));

    close(pipefd[1]);

    _exit(EXIT_SUCCESS);
  } else {  // Parent process
    int left_value, right_value;

    if (node->left->type == OPERATION)
      left_value = parallel_evaluate_node(node->left);
    else
      left_value = node->left->value.num;

    if (node->right->type == OPERATION)
      right_value = parallel_evaluate_node(node->right);
    else
      right_value = node->right->value.num;

    write(pipefd[1], &left_value, sizeof(int));
    write(pipefd[1], &right_value, sizeof(int));
    // Write operation to be computed to pipe
    write(pipefd[1], &node->value.op, sizeof(Operation));

    close(pipefd[1]);

    wait(NULL);

    int result;

    read(pipefd[0], &result, sizeof(int));

    close(pipefd[0]);

    char op_char;

    if (node->value.op == OP_ADD)
      op_char = '+';
    else
      op_char = 'x';

    printf("%d %c %d = %d\n", left_value, op_char, right_value, result);

    return result;
  }
}


int main()
{
  // An example expression string to be processed
  // This string has an unmatched opening brace, but still works!
  char *test_str = "10 x [(2x(5+6)) +(3x(2+3))]x [[(4x (8+5)) + (5x (2+4))]";

  // Read the input into an array of tokens
  Token **tokens = tokenize_input(test_str);

  // Rearrange the input tokens into postfix form
  Token **postfix_tokens = parse_expression(tokens);

  // Convert the postfix tokens into an operation tree
  Node *tree = create_operation_tree(postfix_tokens);

  // Print everything out nice and pretty
  printf("Infix tokens: ");
  print_token_array(tokens);

  printf("Postfix tokens: ");
  print_token_array(postfix_tokens);

  printf("\nOperation Tree:\n");
  print_tree(tree);

  printf("\n");

  int result = parallel_evaluate_node(tree);

  printf("\nResult: %d\n", result);

  return 0;
}
