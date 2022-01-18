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
  stack_iterator(stack_iterator& other)
      : current_head(other.current_head), pool{other.pool} {}
  stack_iterator(stack_iterator&& other)
      : current_head(other.current_head), pool{std::move(other.pool)} {}
  ~stack_iterator() {}

  stack_iterator& operator=(stack_iterator& other) {
    current_head = other.current_head;
    pool = other.pool;
    return *this;
  }

  T&& operator*() const {
    T value = pool.value(current_head);
    return std::move(value);
  }

  stack_iterator& operator++() {
    current_head = pool.next(current_head);
    return *this;
  }
  stack_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  stack_iterator<stack_type, const T, P> const_next() const {
    return pool.cbegin(pool.next(current_head));
  }

  const stack_type as_stack_type() const { return current_head; }

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
      node_t n = node_t();
      n.next = end();
      pool.push_back(std::move(n));
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

  using iterator = stack_iterator<stack_type, T, const stack_pool>;
  using const_iterator = stack_iterator<stack_type, const T, const stack_pool>;

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
    iterator current_node = begin(x);
    while (current_node.const_next() != cend(x))
      ++current_node;

    node(current_node.as_stack_type()).next = free_nodes;
    // the head of the free_nodes stack is now the former head of the old stack
    free_nodes = x;

    // the stack is now empty
    return end();
  }

  void print_stack(std::ostream& os, stack_type head) {
    os << "STACK (head=" << head << ")" << std::endl;
    for (auto it = cbegin(head); it != cend(head); ++it)
      os << *it << " -> " << *it << std::endl;
    os << "END" << std::endl;
  }
};

namespace stack_utils {
  template <typename foreign_iterator, typename value_type, typename stack_type>
  stack_type push_all(stack_pool<value_type, stack_type>& pool,
                      stack_type head,
                      foreign_iterator first,
                      foreign_iterator last) {
    do {
      head = pool.push(*first, head);
    } while (++first != last);
    return head;
  }

  template <typename value_type, typename stack_type>
  std::vector<value_type> to_vector(stack_pool<value_type, stack_type>& pool,
                                    stack_type head) {
    std::vector<value_type> v;
    do {
      v.push_back(std::move(pool.value(head)));
    } while ((head = pool.pop(head)) != pool.end());
    return v;
  }

  template <typename value_type, typename stack_type>
  std::size_t stack_size(stack_pool<value_type, stack_type>& pool,
                         stack_type head) {
    std::size_t size = 0;
    for (auto it = pool.cbegin(head); it != pool.cend(head); ++it)
      ++size;
    return size;
  }
}  // namespace stack_utils
