#include <iostream>
#include <vector>

template <typename T, typename N = std::size_t>
class stack_pool {
 private:
  struct node_t {
    T value;
    N next;
  };

  std::vector<node_t> pool;
  using stack_type = N;
  using value_type = T;
  using size_type = typename std::vector<node_t>::size_type;

  stack_type free_nodes;  // at the beginning, it is empty

  node_t& node(stack_type x) noexcept { return pool[x - 1]; }
  const node_t& node(stack_type x) const noexcept { return pool[x - 1]; }

  template <typename X>
  stack_type _push(X&& val, stack_type head) {
    if (free_nodes == end()) {
      pool.push_back(node_t());
      free_nodes = pool.size();
    }

    stack_type new_head = free_nodes;
    node_t& new_head_node = node(new_head);

    // we update free_nodes using the next free node
    free_nodes = new_head_node.next;

    new_head_node.next = head;
    new_head_node.value = std::forward<X>(val);

    return new_head;
  }

 public:
  stack_pool() : free_nodes{end()} {};
  explicit stack_pool(size_type n) : free_nodes{end()} { reserve(n); }

  //  using iterator = ...;
  //  using const_iterator = ...;

  // iterator begin(stack_type x);
  // iterator end(stack_type);  // this is not a typo

  // const_iterator begin(stack_type x) const;
  // const_iterator end(stack_type) const;

  // const_iterator cbegin(stack_type x) const;
  // const_iterator cend(stack_type) const;

  stack_type new_stack() { return end(); }

  void reserve(size_type n) { pool.reserve(n); }
  size_type capacity() const { return pool.capacity(); }

  bool empty(stack_type x) const { return x == end(); }

  stack_type end() const noexcept { return stack_type(0); }

  T& value(stack_type x) { return node(x).value; }
  const T& value(stack_type x) const { return node(x).value; }

  stack_type& next(stack_type x) { return node(x).next; }
  const stack_type& next(stack_type x) const { return node(x).next; }

  stack_type push(const T& val, stack_type head) { return _push(val, head); }
  stack_type push(T&& val, stack_type head) {
    return _push(std::move(val), head);
  }

  stack_type pop(stack_type x) {
    // the head is an empty node "allocated" for a stack
    node_t& head = node(x);
    stack_type new_head = head.next;

    // the newly freed node becomes the head of the stack of free nodes
    head.next = free_nodes;
    free_nodes = x;

    return new_head;
  }

  stack_type free_stack(stack_type x) {
    if (x == end())
      return x;

    // we look for the bottom-element of this stack, and make it point to the
    // head of the stack free_nodes
    node_t& bottom = node(x);
    // we stop when next is end()
    while ((bottom = node(bottom.next)).next != end())
      ;

    bottom.next = free_nodes;
    // the head of the free_nodes stack is now the former head of the old stack
    free_nodes = x;

    // the stack is now empty
    return end();
  }

  void print_stack(std::ostream& os, stack_type head) {
    os << "STACK (head=" << head << ")" << std::endl;
    while (head != end()) {
      node_t& head_node = node(head);
      os << head << " -> " << head_node.value << std::endl;
      head = head_node.next;
    }
    os << "END" << std::endl;
  }
};
