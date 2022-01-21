#include "catch.hpp"

#include "stack_pool.hpp"
#include <algorithm>  // max_element, min_element
#include <vector>

SCENARIO("getting confident with the addresses") {
  stack_pool<int, std::size_t> pool{16};
  auto l = pool.new_stack();

  REQUIRE(l == 0);
  l = pool.push(42, l);
  REQUIRE(l == std::size_t(1));
}

SCENARIO("basic operations on stack") {
  GIVEN("a generic pool") {
    stack_pool<int, std::size_t> pool{16};

    GIVEN("a new stack") {
      auto l = pool.new_stack();

      THEN("the stack is empty")
      REQUIRE(pool.empty(l));

      WHEN("we add one value") {
        l = pool.push(1, l);
        REQUIRE(pool.value(l) == 1);

        WHEN("we delete the only node") {
          l = pool.pop(l);
          THEN("the stack is empty again")
          REQUIRE(pool.empty(l));

          // empty stacks point to pool.end()
          REQUIRE(l == pool.end());
        }
      }

      WHEN("we add two values") {
        l = pool.push(1, l);
        REQUIRE(pool.value(l) == 1);
        l = pool.push(2, l);

        REQUIRE(pool.value(l) == 2);

        l = pool.free_stack(l);
        REQUIRE(pool.empty(l) == true);
      }
    }
  }
}

SCENARIO("handling multiple stacks") {
  GIVEN("a simple pool") {
    stack_pool<int, uint16_t> pool{};
    WHEN("we create two stacks") {
      auto l1 = pool.new_stack();
      l1 = pool.push(3, l1);
      l1 = pool.push(2, l1);
      l1 = pool.push(1, l1);

      auto l2 = pool.new_stack();

      l2 = pool.push(4, l2);
      l2 = pool.push(5, l2);

      // scan l1
      auto tmp = l1;
      REQUIRE(pool.value(tmp) == 1);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 2);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 3);
      tmp = pool.next(tmp);
      REQUIRE(tmp == pool.end());

      // scan l2
      tmp = l2;
      REQUIRE(pool.value(tmp) == 5);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 4);

      tmp = pool.next(tmp);
      REQUIRE(tmp == pool.end());
    }
  }
}

SCENARIO("using nodes of a deleted stack") {
  GIVEN("a stack") {
    stack_pool<int, uint16_t> pool{};
    auto l1 = pool.new_stack();
    l1 = pool.push(3, l1);
    l1 = pool.push(2, l1);
    l1 = pool.push(1, l1);

    auto capacity = pool.capacity();

    WHEN("we delete the stack") {
      l1 = pool.free_stack(l1);

      THEN("l1 is now empty") { REQUIRE(pool.empty(l1)); }

      WHEN("we create a new stack we use the old nodes") {
        auto l2 = pool.new_stack();

        l2 = pool.push(4, l2);
        l2 = pool.push(5, l2);

        auto tmp = l2;
        REQUIRE(pool.value(tmp) == 5);
        tmp = pool.next(tmp);
        REQUIRE(pool.value(tmp) == 4);

        tmp = pool.next(tmp);
        REQUIRE(tmp == pool.end());

        THEN("since we added just two nodes, the capacity didn't change")
        REQUIRE(pool.capacity() == capacity);
      }
    }
  }
}

SCENARIO("using iterators") {
  GIVEN("two stacks") {
    stack_pool<int, uint16_t> pool{};
    auto l1 = pool.new_stack();
    // credits: pi as random number generator :)
    l1 = pool.push(3, l1);
    l1 = pool.push(1, l1);
    l1 = pool.push(4, l1);
    l1 = pool.push(1, l1);
    l1 = pool.push(5, l1);
    l1 = pool.push(9, l1);
    l1 = pool.push(2, l1);
    l1 = pool.push(6, l1);
    l1 = pool.push(5, l1);
    l1 = pool.push(3, l1);
    l1 = pool.push(5, l1);

    auto l2 = pool.new_stack();
    l2 = pool.push(8, l2);
    l2 = pool.push(9, l2);
    l2 = pool.push(7, l2);
    l2 = pool.push(9, l2);
    l2 = pool.push(3, l2);
    l2 = pool.push(1, l2);
    l2 = pool.push(1, l2);
    l2 = pool.push(5, l2);
    l2 = pool.push(9, l2);
    l2 = pool.push(9, l2);
    l2 = pool.push(7, l2);

    THEN("find the max on l1") {
      auto m = std::max_element(pool.begin(l1), pool.end(l1));
      REQUIRE(*m == 9);
    }

    THEN("find the max on l1 with const iterator") {
      const auto cpool = pool;
      stack_iterator<uint16_t, const int, const stack_pool<int, uint16_t>> m =
          std::max_element(cpool.begin(l1), cpool.end(l1));
      REQUIRE(*m == 9);
    }

    THEN("find the max on l1 with c-iterator") {
      auto m = std::max_element(pool.cbegin(l1), pool.cend(l1));
      REQUIRE(*m == 9);
    }

    THEN("find the min on l2") {
      auto m = std::min_element(pool.begin(l2), pool.end(l2));
      REQUIRE(*m == 1);
    }

    THEN("find the min on l2 with const iterator") {
      const auto cpool = pool;
      stack_iterator<uint16_t, const int, const stack_pool<int, uint16_t>> m =
          std::min_element(cpool.begin(l2), cpool.end(l2));
      REQUIRE(*m == 1);
    }

    THEN("find the min on l2 with c-iterator") {
      auto m = std::min_element(pool.cbegin(l2), pool.cend(l2));
      REQUIRE(*m == 1);
    }
  }
}

SCENARIO("stack_utils namespace") {
  stack_pool<int, std::size_t> pool;
  auto l = pool.new_stack();
  auto l2 = pool.new_stack();

  std::vector<int> v;
  v.push_back(10);
  v.push_back(20);
  v.push_back(10);
  v.push_back(30);

  THEN("push_all pushes everything") {
    l = stack_utils::push_all(pool, l, v.begin(), v.end());
    REQUIRE(pool.value(4) == 30);
    REQUIRE(pool.value(3) == 10);
    REQUIRE(pool.value(2) == 20);
    REQUIRE(pool.value(1) == 10);
    REQUIRE(!pool.empty(l));

    l = stack_utils::push_all(pool, l, v.begin(), v.end());
    REQUIRE(pool.value(8) == 30);
    REQUIRE(pool.value(7) == 10);
    REQUIRE(pool.value(6) == 20);
    REQUIRE(pool.value(5) == 10);
    REQUIRE(pool.value(4) == 30);
    REQUIRE(pool.value(3) == 10);
    REQUIRE(pool.value(2) == 20);
    REQUIRE(pool.value(1) == 10);

    l2 = stack_utils::push_all(pool, l2, v.begin(), v.end());
    REQUIRE(pool.value(12) == 30);
    REQUIRE(pool.value(11) == 10);
    REQUIRE(pool.value(10) == 20);
    REQUIRE(pool.value(9) == 10);
    REQUIRE(!pool.empty(l2));
  }

  THEN("to_vector") {
    l = stack_utils::push_all(pool, l, v.begin(), v.end());
    l2 = stack_utils::push_all(pool, l2, v.begin(), v.end());

    std::vector<int> v2 = stack_utils::to_vector(pool, l2);
    REQUIRE(v2.size() == 4);
    REQUIRE(v2[0] == 30);
    REQUIRE(v2[1] == 10);
    REQUIRE(v2[2] == 20);
    REQUIRE(v2[3] == 10);
  }

  THEN("stack_size gives correct size") {
    l = stack_utils::push_all(pool, l, v.begin(), v.end());
    l = stack_utils::push_all(pool, l, v.begin(), v.end());
    l2 = stack_utils::push_all(pool, l2, v.begin(), v.end());
    REQUIRE(stack_utils::stack_size(pool, l2) == 4);
    REQUIRE(stack_utils::stack_size(pool, l) == 8);
  }
}
