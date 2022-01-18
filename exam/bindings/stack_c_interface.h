#ifndef _CLASS_C_INTERFACE_H_
#define _CLASS_C_INTERFACE_H_

typedef void* stack_pool_c;
typedef unsigned long stack_type;

#ifdef __cplusplus
extern "C" {
#endif

stack_pool_c create_empty_stack_pool();
stack_pool_c create_stack_pool(int n);
void delete_stack_pool(stack_pool_c pool);
stack_type new_stack(stack_pool_c pool);
stack_type stack_end(stack_pool_c pool);
int stack_size(stack_pool_c pool, stack_type head);
stack_type push_to_stack(int value, stack_pool_c pool, stack_type head);
stack_type pop_from_stack(stack_pool_c pool, stack_type head);
int value_from_stack(stack_pool_c pool, stack_type head);

#ifdef __cplusplus
}
#endif

#endif /* _CLASS_C_INTERFACE_H_ */
