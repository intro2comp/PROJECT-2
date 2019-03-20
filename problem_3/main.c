#include <stdio.h>

#include "parser.h"
#include "tree.h"


/* Print a single expression token */
void print_token(Token *token)
{
  switch (token->type) {
    case NUMBER:
      printf("[%d]", token->value);
      break;
    case ADDITION:
      printf("+");
      break;
    case MULTIPLICATION:
      printf("x");
      break;
    case OPEN_PAREN:
      printf("(");
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


int main()
{
  // An example expression string to be processed
  char *test_str = "(5 + 2) x (100 x (3 + 4))";

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

  return 0;
}
