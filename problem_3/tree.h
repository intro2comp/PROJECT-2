#ifndef PARSER_H
#define PARSER_H

#include "parser.h"

#endif


typedef enum {
  VALUE,
  OPERATION
} NodeType;

typedef enum {
  OP_ADD,
  OP_MUL
} Operation;

typedef union {
  int num;
  Operation op;
} NodeValue;

typedef struct Node {
  NodeType type;
  NodeValue value;
  struct Node *left;
  struct Node *right;
} Node;

Node *create_operation_tree(Token **tokens);
