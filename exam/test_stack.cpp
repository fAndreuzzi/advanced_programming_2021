#include "stack_pool.hpp"
#include <iostream>

void test1() {
  stack_pool<int, std::size_t> pool{};

  auto l = pool.new_stack();
  std::cout << (l == pool.end()) << std::endl;
  std::cout << (l == std::size_t(0)) << std::endl;

  l = pool.push(42, l);
  std::cout << l << std::endl;
  std::cout << pool.value(l) << std::endl;
  pool.value(l) = 77;
  std::cout << pool.value(l) << std::endl;
}

void test2() {
  stack_pool<int, std::size_t> pool{};
  auto l = pool.new_stack();
  l = pool.push(10, l);
  l = pool.push(11, l);
  pool.print_stack(std::cout, l);

  auto l2 = pool.new_stack();
  l2 = pool.push(20, l2);
  l = pool.pop(l);
  l2 = pool.push(21, l2);
  pool.print_stack(std::cout, l2);
}

int main(int argc, char** argv) {
  test1();
  std::cout << "------- End of test 1" << std::endl;
  test2();
  std::cout << "------- End of test 2" << std::endl;
}
