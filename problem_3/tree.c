#include <stdlib.h>

#include "stack.h"

#include "tree.h"

#define BUFFER_SIZE 128

/* Read an array of postfix tokens into an evaluation tree */
Node *create_operation_tree(Token **tokens)
{
  int token_index = 0;

  Token *current_token = tokens[0];

  Node **node_array = malloc(sizeof(Node*) * BUFFER_SIZE);

  Stack *node_stack = stack_init(BUFFER_SIZE);

  while (current_token) {
    Node *node = malloc(sizeof(Node));

    node_array[token_index] = node;

    switch (current_token->type) {
      case NUMBER:
        node->type = VALUE;
        node->value.num = current_token->value;
        node->left = NULL;
        node->right = NULL;
        break;
      case ADDITION:
        node->type = OPERATION;
        node->value.op = OP_ADD;
        node->right = node_array[stack_pop(node_stack)];
        node->left = node_array[stack_pop(node_stack)];
        break;
      case MULTIPLICATION:
        node->type = OPERATION;
        node->value.op = OP_MUL;
        node->right = node_array[stack_pop(node_stack)];
        node->left = node_array[stack_pop(node_stack)];
        break;
      default:
        current_token = tokens[++token_index];
        continue;
    }

    stack_push(node_stack, token_index);

    current_token = tokens[++token_index];
  }

  Node *root = node_array[stack_pop(node_stack)];

  return root;
}
