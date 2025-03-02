#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "types.hpp"

constexpr auto cmp = [](side_t side) {
  return [side](const order_t& lhs, const order_t& rhs) {
    return (side == BID ? lhs.price < rhs.price : lhs.price > rhs.price) ||
           (lhs.price == rhs.price && lhs.id > rhs.id);
  };
};

void pop(std::vector<order_t>& orders, side_t side) {
  std::ranges::pop_heap(orders, cmp(side));
  orders.pop_back();
}

void push(std::vector<order_t>& orders, const order_t& order) {
  orders.push_back(order);
  std::ranges::push_heap(orders, cmp(order.side));
}

void swim_up(std::vector<order_t>& orders, side_t side, size_t i) {
  while (i > 0) {
    if (cmp(side)(orders[(i - 1) / 2], orders[i])) {
      std::swap(orders[(i - 1) / 2], orders[i]);
      i = (i - 1) / 2;
    } else {
      break;
    }
  }
}

void swim_down(std::vector<order_t>& orders, side_t side, size_t i) {
  while (i < orders.size()) {
    size_t l = (2 * i) + 1;
    size_t r = l + 1;
    size_t j{};
    if (l >= orders.size() && r >= orders.size()) {
      break;
    }
    if (l >= orders.size()) {
      j = r;
    } else if (r >= orders.size()) {
      j = l;
    } else {
      j = cmp(side)(orders[l], orders[r]) ? r : l;
    }
    if (cmp(side)(orders[i], orders[j])) {
      std::swap(orders[i], orders[j]);
      i = j;
    } else {
      break;
    }
  }
}

std::optional<order_t> remove(std::vector<order_t>& orders, order_id_t order_id,
                              side_t side) {
  assert(std::ranges::is_heap(orders, cmp(side)));
  auto iter = std::ranges::find_if(orders, [order_id](const order_t& order) {
    return order.id == order_id;
  });
  // if element doesn't exist, do nothing
  if (iter == orders.end()) {
    return std::nullopt;
  }
  // if element is already at back, just remove it and we're done
  if (iter == std::prev(orders.end())) {
    auto res = orders.back();
    orders.pop_back();
    return res;
  }
  size_t i = iter - orders.begin();
  auto to_swap = orders.back();
  std::swap(orders[i], orders[orders.size() - 1]);
  auto removed = orders.back();
  orders.pop_back();
  if (cmp(side)(removed, to_swap)) {
    std::cout << "swimming up\n";
    swim_up(orders, side, i);
  } else {
    swim_down(orders, side, i);
  }
  assert(std::ranges::is_heap(orders, cmp(side)));
  return removed;
}
