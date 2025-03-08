#include <algorithm>
#include <cassert>
#include <vector>

#include "types.hpp"

namespace heap {
static constexpr auto CMP = [](side_t side) {
  return [side](const order_t& lhs, const order_t& rhs) {
    return (side == BID ? lhs.price < rhs.price : lhs.price > rhs.price) ||
           (lhs.price == rhs.price && lhs.id > rhs.id);
  };
};

// NOLINTBEGIN (unused-function)
static void pop(std::vector<order_t>& orders, side_t side) {
  std::ranges::pop_heap(orders, CMP(side));
  orders.pop_back();
}

static void push(std::vector<order_t>& orders, const order_t& order) {
  orders.push_back(order);
  std::ranges::push_heap(orders, CMP(order.side));
}

static void swim_up(std::vector<order_t>& orders, side_t side, size_t i) {
  while (i > 0) {
    if (CMP(side)(orders[(i - 1) / 2], orders[i])) {
      std::swap(orders[(i - 1) / 2], orders[i]);
      i = (i - 1) / 2;
    } else {
      break;
    }
  }
}

static void swim_down(std::vector<order_t>& orders, side_t side, size_t i) {
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
      j = CMP(side)(orders[l], orders[r]) ? r : l;
    }
    if (CMP(side)(orders[i], orders[j])) {
      std::swap(orders[i], orders[j]);
      i = j;
    } else {
      break;
    }
  }
}

static std::optional<order_t> remove(std::vector<order_t>& orders,
                                     order_id_t order_id, side_t side) {
  assert(std::ranges::is_heap(orders, CMP(side)));
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
  size_t i = static_cast<size_t>(iter - orders.begin());
  auto to_swap = orders.back();
  std::swap(orders[i], orders[orders.size() - 1]);
  auto removed = orders.back();
  orders.pop_back();
  if (CMP(side)(removed, to_swap)) {
    swim_up(orders, side, i);
  } else {
    swim_down(orders, side, i);
  }
  assert(std::ranges::is_heap(orders, CMP(side)));
  return removed;
}
// NOLINTEND (unused-function)
}  // namespace heap
