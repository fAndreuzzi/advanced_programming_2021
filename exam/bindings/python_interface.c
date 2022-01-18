#include "stack_c_interface.h"

stack_pool_c stack_pool = 0;

void delete_pool() {
  delete_stack_pool(stack_pool);
}

void empty_pool() {
  delete_pool();
  stack_pool = create_empty_stack_pool();
}

void pool(int n) {
  delete_pool();
  stack_pool = create_stack_pool(n);
}

stack_type stack() {
  return new_stack(stack_pool);
}

stack_type end() {
  return stack_end(stack_pool);
}

int size(stack_type head) {
  return stack_size(stack_pool, head);
}

stack_type push(int value, stack_type head) {
  return push_to_stack(value, stack_pool, head);
}

stack_type pop(stack_type head) {
  return pop_from_stack(stack_pool, head);
}

int value(stack_type head) {
  return value_from_stack(stack_pool, head);
}
