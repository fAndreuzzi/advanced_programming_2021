#include <iostream>
#include <iterator>
#include <vector>

template <typename stack_type, typename T, typename P>
class stack_iterator {
  stack_type current_head;
  P* pool;

 public:
  using value_type = T;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  stack_iterator(stack_type head, P* const pool_ptr)
      : current_head{head}, pool{pool_ptr} {
    // check the pool
    if (pool_ptr == nullptr)
      throw std::invalid_argument("The given pool points to nullptr.");

    // check the head
    if (head != pool_ptr->end()) {
      // this is going to throw an exception in case the given head is
      // invalid.
      *(*this);
    }
  }
  stack_iterator(stack_iterator& other)
      // delegating constructor
      : stack_iterator(other.current_head, other.pool) {}
  stack_iterator(stack_iterator&& other)
      : stack_iterator(other.current_head, other.pool) {
    other.current_head = other.pool->end();
    other.pool = nullptr;
  }

  stack_iterator& operator=(stack_iterator& other) noexcept {
    current_head = other.current_head;
    pool = other.pool;
    return *this;
  }

  stack_iterator& operator=(stack_iterator&& other) noexcept {
    current_head = std::move(other.current_head);
    pool = other.pool;

    other.current_head = other.pool->end();
    other.pool = nullptr;

    return *this;
  }

  // we do not need to do anything since we do not hold the pool, someone else
  // has to delete that instance.
  ~stack_iterator() noexcept {}

  // we're sure that this is non-throwing if the pool is not modified during
  // a visit.
  T& operator*() const noexcept(std::is_const<P>::value) {
    return pool->value(current_head);
  }

  // this throws an exception if current_head does not have a next (i.e. it is
  // pool.end)
  stack_iterator& operator++() {
    current_head = pool->next(current_head);
    return *this;
  }
  stack_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  stack_type ptr_to_stack() const noexcept { return current_head; }

  friend bool operator==(const stack_iterator& a,
                         const stack_iterator& b) noexcept {
    return a.current_head == b.current_head;
  }
  friend bool operator!=(const stack_iterator& a,
                         const stack_iterator& b) noexcept {
    return !(a == b);
  }
};

/**
 * @brief A pool which can handle multiple stacks.
 *
 * This pool uses an underlying std::vector to handle multiple stacks at the
 * same time. New elements are added using std::vector::push_back"()", therefore
 * re-allocations may occur when needed. For this reason it is advised to use
 * stack_pool::reserve and to forecast multiple allocations before they occur
 * to improve performance.
 *
 * It is strongly recommended not to ignore returned values from the functions
 * of stack_pool. For instance, stack_pool::push and stack_pool::pop return the
 * head of the new stack: using the former head will result 100% in unintended
 * behaviors and unpredictable errors, since elements popped from a stack are
 * not reset to default values (but are inserted into a stack of "free nodes").
 *
 *
 * @tparam T Type of the values to be held in the stacks of this pool.
 * @tparam N Type using to designate the head of a stack.
 */
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

  node_t& node(stack_type x) { return pool.at(x - 1); }
  const node_t& node(stack_type x) const { return pool.at(x - 1); }

  template <typename X>
  // universal reference
  // this is marked as noexcept because it is not possible to get an exception,
  // since the allocation of a new free node is managed internally, and head
  // is just used as the new "next node" of the free node in which the new
  // value is placed.
  stack_type _push(X&& val, stack_type head) noexcept {
    // if needed, we allocate a new free node
    if (free_nodes == end()) {
      node_t n = node_t();
      n.next = end();
      pool.push_back(std::move(n));
      free_nodes = pool.size();
    }

    stack_type new_head = free_nodes;

    // we use the next free node as the new head of the stack free_nodes
    free_nodes = next(free_nodes);

    next(new_head) = head;
    value(new_head) = std::forward<X>(val);

    return new_head;
  }

 public:
  /**
   * @brief Construct a new stack pool object having initial capacity 0.
   *
   */
  stack_pool() noexcept : free_nodes{end()} {};

  /**
   * @brief Construct a new stack pool object having a given initial capacity.
   *
   * @param n The initial capacity of the pool.
   */
  explicit stack_pool(size_type n) : free_nodes{end()} { reserve(n); }

  using iterator = stack_iterator<stack_type, T, stack_pool>;
  using const_iterator = stack_iterator<stack_type, const T, const stack_pool>;

  iterator begin(stack_type x) { return iterator(x, this); }
  iterator end(stack_type) noexcept { return iterator(end(), this); }

  const_iterator begin(stack_type x) const { return const_iterator(x, this); }
  const_iterator end(stack_type) const noexcept {
    return const_iterator(end(), this);
  }

  const_iterator cbegin(stack_type x) const { return const_iterator(x, this); }
  const_iterator cend(stack_type) const noexcept {
    return const_iterator(end(), this);
  }

  /**
   * @brief "Allocate" a new stack in this pool. Returns the head of the new
   * stack.
   *
   * @return stack_type
   */
  stack_type new_stack() noexcept { return end(); }

  /**
   * @brief Advise the pool to allocate some more space.
   *
   * This method might be useful to improve performance when adding multiple
   * elements all in once to the pool.
   *
   * This method throws an exception if n > vector::max_size.
   *
   * @param n The advised new size of the stack.
   */
  void reserve(size_type n) { pool.reserve(n); }

  /**
   * @brief Return the capacity of the pool (i.e. the total number of stack
   * elements it can hold without reallocating space).
   *
   * @return size_type
   */
  size_type capacity() const noexcept { return pool.capacity(); }

  /**
   * @brief Check whether the given stack is empty.
   *
   * @param x Head of the stack.
   * @return true If the stack is empty.
   * @return false Otherwise.
   */
  bool empty(stack_type x) const noexcept { return x == end(); }

  /**
   * @brief Common end (i.e. after last node) of all the stacks in this pool.
   *
   * @return stack_type
   */
  stack_type end() const noexcept { return stack_type(0); }

  /**
   * @brief Return the front value in the given stack.
   *
   * This method throws an exception if the given head is not a invalid index
   * in the pool.
   *
   * @param x
   * @return T&
   */
  T& value(stack_type x) { return node(x).value; }
  /**
   * @brief Return the front value in the given stack.
   *
   * This method throws an exception if the given head is not a invalid index
   * in the pool.
   *
   * @param x
   * @return T&
   */
  const T& value(stack_type x) const { return node(x).value; }

  /**
   * @brief Return the next node in the given stack.
   *
   * This method throws an exception if the given head is not a invalid index
   * in the pool.
   *
   * @param x
   * @return stack_type&
   */
  stack_type& next(stack_type x) { return node(x).next; }
  /**
   * @brief Return the next node in the given stack.
   *
   * This method throws an exception if the given head is not a invalid index in
   * the pool.
   *
   * @param x
   * @return const stack_type&
   */
  const stack_type& next(stack_type x) const { return node(x).next; }

  /**
   * @brief Push an element to the front of the stack. Returns the new head of
   * the stack.
   *
   * The pool does not check if the given head is actually the head of a stack
   * (or even a valid index of the pool) therefore it is up to the user to use
   * the pool properly.
   *
   * @param val Value to be pushed.
   * @param head Head of the stack.
   * @return stack_type
   */
  stack_type push(const T& val, stack_type head) noexcept {
    return _push(val, head);
  }

  /**
   * @brief Push an element to the front of the stack. Returns the new head of
   * the stack.
   *
   * The pool does not check if the given head is actually the head of a stack
   * (or even a valid index of the pool) therefore it is up to the user to use
   * the pool properly.
   *
   * @param val Value to be pushed.
   * @param head Head of the stack.
   * @return stack_type
   */
  stack_type push(T&& val, stack_type head) noexcept {
    return _push(std::move(val), head);
  }

  /**
   * @brief Pop the head of the given stack.
   *
   * This method throws an exception if the given head is not a invalid index
   * in the pool (i.e. negative index or bigger than the size of the pool).
   *
   * The pool does not check if the given head is actually the head of a stack,
   * therefore it is up to the user to use the pool properly. In case a non-head
   * index is popped, a kind-of memory leak occur, since the front part of the
   * stack becomes de-facto unreachable by the pool.
   *
   * @param head Head of the stack from which we intend to pop the front
   *                element.
   * @return stack_type
   */
  stack_type pop(stack_type head) {
    stack_type new_stack_head = next(head);

    // the newly freed node becomes the head of the stack of free nodes
    next(head) = free_nodes;
    free_nodes = head;

    return new_stack_head;
  }

  /**
   * @brief Empty the given stack.
   *
   * This method throws an exception if the given head is not a invalid index in
   * the pool (i.e. negative index or bigger than the size of the pool).
   *
   * The pool does not check if the given head is actually the head of a stack,
   * therefore it is up to the user to use the pool properly.
   *
   * @param head Head of the stack to be emptied.
   * @return stack_type
   */
  stack_type free_stack(stack_type head) {
    if (empty(head))
      return head;

    // we look for the bottom-element of this stack, and make it point to the
    // head of the stack free_nodes
    iterator current_node = begin(head);
    iterator next_node = current_node;
    ++next_node;
    while (next_node != end(head))
      current_node = next_node++;

    next(current_node.ptr_to_stack()) = free_nodes;
    // the head of the free_nodes stack is now the former head of the old stack
    free_nodes = head;

    // the stack is now empty
    return end();
  }
};

namespace stack_utils {
  /**
   * @brief Push all the items in the given iterator to the given stack (first
   * to last).
   *
   * This function throws an exception if the given head is not a invalid index
   * in the pool (i.e. negative index or bigger than the size of the pool).
   *
   * The pool does not check if the given head is actually the head of a stack,
   * therefore it is up to the user to use the pool properly.
   *
   * @tparam foreign_iterator Some kind of iterator which delivers values of
   *            type `value_type`.
   * @tparam value_type Type of the values held in the stack pool.
   * @tparam stack_type Type of "pointers" to stack nodes.
   * @param pool Stack pool containing the stack referenced by `head`.
   * @param head Head of the stack to be augmented.
   * @param first An iterator pointing to the first element to be pushed into
   *            the stack.
   * @param last An iterator pointing past the last element to be pushed into
   *            the stack.
   * @return stack_type
   */
  template <typename foreign_iterator, typename value_type, typename stack_type>
  stack_type push_all(stack_pool<value_type, stack_type>& pool,
                      stack_type head,
                      foreign_iterator first,
                      foreign_iterator last) noexcept {
    do {
      head = pool.push(*first, head);
    } while (++first != last);
    return head;
  }

  /**
   * @brief Convert the given stack to an std::vector. The stack is empty
   * afterwards, and should not be mentioned anymore.
   *
   * This function throws an exception if the given head is not a invalid index
   * in the pool (i.e. negative index or bigger than the size of the pool).
   *
   * The pool does not check if the given head is actually the head of a stack,
   * therefore it is up to the user to use the pool properly.
   *
   * @tparam value_type Type of the values held in the stack pool.
   * @tparam stack_type Type of "pointers" to stack nodes.
   * @param pool Stack pool containing the stack referenced by `head`.
   * @param head Head of the stack to be converted.
   * @return std::vector<value_type>
   */
  template <typename value_type, typename stack_type>
  std::vector<value_type> to_vector(stack_pool<value_type, stack_type>& pool,
                                    stack_type head) {
    std::vector<value_type> v;
    if (!pool.empty(head)) {
      do {
        v.push_back(std::move(pool.value(head)));
      } while ((head = pool.pop(head)) != pool.end());
    }
    return v;
  }

  /**
   * @brief Compute the size of the stack starting at the given `head`. The
   * stack is not modified.
   *
   * This function throws an exception if the given head is not a invalid index
   * in the pool (i.e. negative index or bigger than the size of the pool).
   *
   * This function may also be used to find the size of a portion of a stack.
   *
   * @tparam value_type Type of the values held in the stack pool.
   * @tparam stack_type Type of "pointers" to stack nodes.
   * @param pool Stack pool containing the stack referenced by `head`.
   * @param head Head of the stack to be measured.
   * @return std::size_t
   */
  template <typename value_type, typename stack_type>
  std::size_t stack_size(const stack_pool<value_type, stack_type>& pool,
                         const stack_type head) {
    std::size_t size = 0;
    for (auto it = pool.cbegin(head); it != pool.cend(head); ++it)
      ++size;
    return size;
  }

  /**
   * @brief Print the content of the given stack. The stack is not modified.
   *
   * This function throws an exception if the given head is not a invalid index
   * in the pool (i.e. negative index or bigger than the size of the pool).
   *
   * This function may also be used to print only a portion of a stack.
   *
   * @param os An output stream (like `std::cout` or `std::cerr`).
   * @param head The head of the stack to be printed.
   */
  template <typename value_type, typename stack_type>
  void print_stack(std::ostream& os,
                   const stack_pool<value_type, stack_type>& pool,
                   const stack_type head) {
    os << "STACK (head=" << head << ")" << std::endl;
    for (auto it = pool.cbegin(head); it != pool.cend(head); ++it)
      os << it.ptr_to_stack() << " -> " << *it << std::endl;
    os << "END" << std::endl;
  }
}  // namespace stack_utils
