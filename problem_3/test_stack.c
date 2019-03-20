#include <assert.h>
#include <stdio.h>

#include "stack.h"

#define COUNT 8

int main()
{
  Stack *stack = stack_init(COUNT);

  assert(stack->size == COUNT);
  assert(stack->top == -1);

  assert (stack_is_empty(stack));
  assert (!stack_is_full(stack));

  for (int i = 0; i < COUNT; i++) {
    stack_push(stack, i);
    assert(stack->data[i] == i);
  }

  assert(!stack_is_empty(stack));
  assert(stack_is_full(stack));

  assert(stack->top == COUNT - 1);
  assert(COUNT - 1 == stack_peek(stack));

  for (int i = COUNT; i > 0; i--)
    assert(stack_pop(stack) == i - 1);

  assert(stack_is_empty(stack));
  assert(!stack_is_full(stack));

  return 0;
}
