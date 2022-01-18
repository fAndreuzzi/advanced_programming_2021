#include "stack_c_interface.h"
#include "../stack_pool.hpp"

extern "C" {

using int_stack_pool = stack_pool<int, std::size_t>;

stack_pool_c create_empty_stack_pool() {
  return new int_stack_pool();
}

stack_pool_c create_stack_pool(int n) {
  return new int_stack_pool(n);
}

void delete_stack_pool(stack_pool_c pool) {
  delete static_cast<int_stack_pool*>(pool);
}

stack_type stack_end(stack_pool_c pool) {
  return static_cast<int_stack_pool*>(pool)->end();
}

int stack_size(stack_pool_c pool, stack_type head) {
  return stack_utils::stack_size(*static_cast<int_stack_pool*>(pool), head);
}

stack_type new_stack(stack_pool_c pool) {
  return static_cast<int_stack_pool*>(pool)->new_stack();
}

stack_type push_to_stack(int value, stack_pool_c pool, stack_type head) {
  return static_cast<int_stack_pool*>(pool)->push(value, head);
}

stack_type pop_from_stack(stack_pool_c pool, stack_type head) {
  return static_cast<int_stack_pool*>(pool)->pop(head);
}

int value_from_stack(stack_pool_c pool, stack_type head) {
  return static_cast<int_stack_pool*>(pool)->value(head);
}
}
