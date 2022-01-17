#include <iostream>
#include <iterator>
#include <vector>

template <typename stack_type, typename T, typename P>
class stack_iterator {
  stack_type current_head;
  P& pool;

 public:
  using value_type = T;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  stack_iterator(stack_type head, P& spool) : current_head{head}, pool{spool} {}
  ~stack_iterator() {}

  stack_iterator& operator=(stack_iterator& other) {
    current_head = other.current_head;
    pool = other.pool;
    return *this;
  }

  T& operator*() const { return pool.value(current_head); }

  stack_iterator& operator++() {
    current_head = pool.next(current_head);
    return *this;
  }
  stack_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const stack_iterator& a, const stack_iterator& b) {
    return a.current_head == b.current_head;
  }
  friend bool operator!=(const stack_iterator& a, const stack_iterator& b) {
    return !(a == b);
  }
};

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

  stack_type free_nodes;

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

  using iterator = stack_iterator<stack_type, T, stack_pool>;
  using const_iterator = stack_iterator<stack_type, const T, stack_pool>;

  iterator begin(stack_type x) { return iterator(x, *this); }
  iterator end(stack_type x) { return iterator(end(), *this); }

  const_iterator begin(stack_type x) const { return const_iterator(x, *this); }
  const_iterator end(stack_type x) const {
    return const_iterator(end(), *this);
  }

  const_iterator cbegin(stack_type x) const { return const_iterator(x, *this); }
  const_iterator cend(stack_type x) const {
    return const_iterator(end(), *this);
  }

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
