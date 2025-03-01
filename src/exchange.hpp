#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>

#include "models.hpp"

struct exchange_t {
  user_t num_users = 0;
  uint8_t next_asset = CALIFORNIA_ROLL;
  std::atomic<uint32_t> order_id = 0;
  std::mutex all_users_mutex;
  std::array<user_entry_t, MAX_USERS> user_entries = {};
  std::array<std::mutex, MAX_USERS> user_mutexes = {};
  std::array<std::vector<order_t>, NUM_ASSETS> bids = {};
  std::array<std::vector<order_t>, NUM_ASSETS> asks = {};
  std::array<std::mutex, NUM_ASSETS> asset_mutexes = {};

  user_t register_user(price_t cash,
                       std::array<volume_t, NUM_ASSETS> starting_amounts) {
    std::scoped_lock lock(all_users_mutex);
    auto& entry = user_entries[num_users];
    entry.cash_held = cash;
    entry.buying_power = cash;
    entry.amount_held = starting_amounts;
    entry.selling_power = starting_amounts;
    return num_users++;
  }

  user_t register_user() {
    std::scoped_lock lock(all_users_mutex);
    auto& entry = user_entries[num_users];
    entry.amount_held[next_asset] = STARTING_AMOUNTS[next_asset];
    entry.selling_power[next_asset] = STARTING_AMOUNTS[next_asset];
    entry.cash_held = STARTING_CASH[next_asset];
    entry.buying_power = STARTING_CASH[next_asset];
    next_asset = static_cast<asset_t>(next_asset + 1) % NUM_ASSETS;
    return num_users++;
  }

  std::optional<std::string_view> validate_and_reserve(const order_t& order) {
    if (order.user >= num_users) {
      return "Invalid user.";
    }
    if (order.price <= 0) {
      return "Invalid price.";
    }
    if (order.volume <= 0) {
      return "Invalid volume.";
    }
    std::scoped_lock lock(user_mutexes[order.user]);
    auto& user_entry = user_entries[order.user];
    switch (order.side) {
      case BID: {
        price_t order_value = order.price * order.volume;
        if (order_value > user_entry.buying_power) {
          return "Insufficient buying power.";
        }
        user_entry.buying_power -= order_value;
        break;
      }
      case ASK: {
        if (order.volume > user_entry.selling_power[order.asset]) {
          return "Insufficient selling power.";
        }
        user_entry.selling_power[order.asset] -= order.volume;
        break;
      }
    }
    return {};
  }

  void add_order(const order_t& order) {
    auto& orders = (order.side == BID ? bids : asks)[order.asset];
    orders.push_back(order);
    std::ranges::push_heap(orders, [&order](const auto& lhs, const auto& rhs) {
      return order.side == BID ? lhs.price < rhs.price : lhs.price > rhs.price;
    });
  }

  OrderResult match_order(order_t& order) {
    auto& opposing_orders = (order.side == BID ? asks : bids)[order.asset];
    auto is_match = [&order](const order_t& other) {
      return order.side == BID ? order.price >= other.price
                               : order.price <= other.price;
    };
    std::vector<trade_t> trades;
    while (order.volume > 0 && !opposing_orders.empty() &&
           is_match(opposing_orders[0])) {
      auto& other = opposing_orders[0];
      volume_t matched_volume = std::min(order.volume, other.volume);
      order.volume -= matched_volume;
      other.volume -= matched_volume;
      trades.emplace_back(other.id, other.price, matched_volume,
                          order.side == BID ? order.user : other.user,
                          order.side == BID ? other.user : order.user,
                          order.asset);
      if (other.volume == 0) {
        std::ranges::push_heap(
            opposing_orders, [&other](const auto& lhs, const auto& rhs) {
              return other.side == BID ? lhs.price < rhs.price
                                       : lhs.price > rhs.price;
            });
        opposing_orders.pop_back();
      }
    }
    if (order.volume > 0) {
      return {.error = {}, .trades = trades, .unmatched = order};
    }
    return {.error = {}, .trades = trades, .unmatched = {}};
  }

  void execute_trade(side_t taker_side, const trade_t& trade) {
    user_t taker = taker_side == BID ? trade.buyer : trade.seller;
    user_t maker = taker_side == BID ? trade.seller : trade.buyer;
    uint32_t order_cost = trade.price * trade.volume;
    switch (taker_side) {
      case BID:
        user_entries[maker].cash_held += order_cost;
        user_entries[maker].buying_power += order_cost;
        user_entries[taker].cash_held -= order_cost;
        // user_entries[taker].buying_power -= order_cost;

        user_entries[maker].amount_held[trade.asset] -= trade.volume;
        // user_entries[maker].selling_power[asset] -= volume;
        user_entries[taker].amount_held[trade.asset] += trade.volume;
        user_entries[taker].selling_power[trade.asset] += trade.volume;
        break;
      case ASK:
        user_entries[maker].cash_held -= order_cost;
        // user_entries[maker].buying_power -= order_cost;
        user_entries[taker].cash_held += order_cost;
        user_entries[taker].buying_power += order_cost;

        user_entries[maker].amount_held[trade.asset] += trade.volume;
        user_entries[maker].selling_power[trade.asset] += trade.volume;
        user_entries[taker].amount_held[trade.asset] -= trade.volume;
        // user_entries[taker].selling_power[asset] -= volume;
        break;
    }
  }

  void execute_trades(side_t taker_side, const std::vector<trade_t>& trades) {
    if (trades.empty()) {
      return;
    }
    for (const auto& trade : trades) {
      if (trade.buyer == trade.seller) {
        std::scoped_lock lock(user_mutexes[trade.buyer]);
        execute_trade(taker_side, trade);
      } else {
        // when executing trades, ensure that we acquire the lock for the user with
        // the lowest id first to avoid deadlock
        std::scoped_lock lock_min(
            user_mutexes[std::min(trade.buyer, trade.seller)]);
        std::scoped_lock lock_max(
            user_mutexes[std::max(trade.buyer, trade.seller)]);
        execute_trade(taker_side, trade);
      }
    }
  }

  OrderResult place_order(order_t& order) {
    std::optional<std::string_view> err = validate_and_reserve(order);
    if (err.has_value()) {
      return {.error = err, .trades = {}, .unmatched = {}};
    }
    order.id = order_id++;
    OrderResult result{};
    {
      std::scoped_lock lock(asset_mutexes[order.asset]);
      result = match_order(order);
      if (result.unmatched.has_value()) {
        add_order(result.unmatched.value());
      }
    }
    execute_trades(order.side, result.trades);
    return result;
  }
};
