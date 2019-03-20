#include <stdlib.h> // malloc
#include <stdio.h> // printf

#include "stack.h"

/* Allocate a new stack of *max_size* itmes on the heap */
Stack *stack_init(int max_size)
{
  Stack *stack = malloc(sizeof(Stack) + sizeof(int) * max_size);

  stack->top = -1;
  stack->max_size = max_size;

  return stack;
}


/* Deallocate a stack */
void stack_destroy(Stack *stack)
{
  free(stack);
}


/* Determine if a stack is empty */
int stack_is_empty(Stack *stack)
{
  if (stack->top == -1)
    return 1;
  else
    return 0;
}


/* Determine if a stack is full */
int stack_is_full(Stack *stack)
{
  if (stack->top == stack->max_size - 1)
    return 1;
  else
    return 0;
}


/* Return the item on the top of the stack without popping */
int stack_peek(Stack *stack)
{
  if (stack->top == -1) {
    printf("Could peek because Stack is empty.\n");
    exit(1);
  }

  return stack->data[stack->top];
}


/* Push a value onto the stack */
void stack_push(Stack *stack, int value)
{
  if (!stack_is_full(stack)) {
    stack->top = stack->top + 1;
    stack->data[stack->top] = value;
  } else {
    printf("Could not insert data, Stack is full.\n");
    exit(1);
  }
}


/* Pop a value off the top of the stack */
int stack_pop(Stack *stack)
{
  int value;

  if (!stack_is_empty(stack)) {
    value = stack->data[stack->top];
    stack->top = stack->top - 1;
    return value;
  } else {
    printf("Could not retrieve data, Stack is empty.\n");
    exit(1);
  }
}

