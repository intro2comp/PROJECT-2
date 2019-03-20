#include <assert.h>
#include <stdio.h>

#include "queue.h"

#define COUNT 8

int main()
{
  Queue *queue = queue_init(COUNT);


  assert(queue_is_empty(queue));
  assert(!queue_is_full(queue));

  for (int i = 0; i < COUNT; i++)
    queue_push(queue, i);

  assert(!queue_is_empty(queue));
  assert(queue_is_full(queue));

  for (int i = 0; i < COUNT; i++)
    assert(queue_pop(queue) == i);

  assert(queue_is_empty(queue));
  assert(!queue_is_full(queue));

  return 0;
}
