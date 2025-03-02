#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

#include "heap.hpp"
#include "types.hpp"

struct exchange_t {
  user_t num_users = 0;
  uint8_t next_asset = CALIFORNIA_ROLL;
  order_id_t order_id = 0;
  std::array<user_entry_t, MAX_USERS> user_entries = {};
  std::array<std::vector<order_t>, NUM_ASSETS> bids = {};
  std::array<std::vector<order_t>, NUM_ASSETS> asks = {};

  bool verify_state() {
    for (user_t user = 0; user < num_users; ++user) {
      auto& user_entry = user_entries[user];
      price_t expected_buying_power = user_entry.cash_held;
      for (const auto& heap : bids) {
        for (const auto& order : heap) {
          if (order.user == user) {
            expected_buying_power -= order.price * order.volume;
          }
        }
        assert(std::ranges::is_heap(heap, cmp(BID)));
      }
      assert(expected_buying_power == user_entry.buying_power);

      for (asset_t asset = CALIFORNIA_ROLL; asset < NUM_ASSETS;
           asset = static_cast<asset_t>(asset + 1)) {
        volume_t expected_selling_power = user_entry.amount_held[asset];
        for (const auto& order : asks[asset]) {
          if (order.user == user) {
            expected_selling_power -= order.volume;
          }
        }

        assert(std::ranges::is_heap(asks[asset], cmp(ASK)));
        assert(expected_selling_power == user_entry.selling_power[asset]);
      }
    }
    return true;
  }

  [[nodiscard]] user_t register_user(
      price_t cash, std::array<volume_t, NUM_ASSETS> starting_amounts) {
    auto& entry = user_entries[num_users];
    entry.cash_held = cash;
    entry.buying_power = cash;
    entry.amount_held = starting_amounts;
    entry.selling_power = starting_amounts;
    return num_users++;
  }

  [[nodiscard]] user_t register_user() {
    auto& entry = user_entries[num_users];
    entry.amount_held[next_asset] = STARTING_AMOUNTS[next_asset];
    entry.selling_power[next_asset] = STARTING_AMOUNTS[next_asset];
    entry.cash_held = STARTING_CASH[next_asset];
    entry.buying_power = STARTING_CASH[next_asset];
    next_asset = static_cast<asset_t>(next_asset + 1) % NUM_ASSETS;
    return num_users++;
  }

  [[nodiscard]] std::optional<std::string_view> validate_order(
      const order_t& order) {
    if (order.user >= num_users) {
      return "Invalid user.";
    }
    if (order.price <= 0) {
      return "Invalid price.";
    }
    if (order.volume <= 0) {
      return "Invalid volume.";
    }
    const auto& user_entry = user_entries[order.user];
    switch (order.side) {
      case BID:
        if (order.price * order.volume > user_entry.buying_power) {
          return "Insufficient buying power.";
        }
        break;
      case ASK:
        if (order.volume > user_entry.selling_power[order.asset]) {
          return "Insufficient selling power.";
        }
        break;
    }
    return std::nullopt;
  }

  void add_order(const order_t& order) {
    switch (order.side) {
      case BID:
        user_entries[order.user].buying_power -= order.price * order.volume;
        break;
      case ASK:
        user_entries[order.user].selling_power[order.asset] -= order.volume;
        break;
    }
    auto& orders = (order.side == BID ? bids : asks)[order.asset];
    push(orders, order);
  }

  [[nodiscard]] OrderResult match_order(order_t& order) {
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
        pop(opposing_orders, other.side);
      }
    }
    if (order.volume > 0) {
      return {.error = {}, .trades = trades, .unmatched = order};
    }
    return {.error = {}, .trades = trades, .unmatched = {}};
  }

  void execute_trade(const order_t& incoming_order, const trade_t& trade) {
    user_t taker = incoming_order.side == BID ? trade.buyer : trade.seller;
    user_t maker = incoming_order.side == BID ? trade.seller : trade.buyer;
    uint32_t trade_cost = trade.price * trade.volume;
    switch (incoming_order.side) {
      case BID:
        user_entries[maker].cash_held += trade_cost;
        user_entries[maker].buying_power += trade_cost;
        user_entries[taker].cash_held -= trade_cost;
        user_entries[taker].buying_power -= trade_cost;

        user_entries[maker].amount_held[trade.asset] -= trade.volume;
        // user_entries[maker].selling_power[trade.asset] -= trade.volume;
        user_entries[taker].amount_held[trade.asset] += trade.volume;
        user_entries[taker].selling_power[trade.asset] += trade.volume;
        break;
      case ASK:
        user_entries[maker].cash_held -= trade_cost;
        // user_entries[maker].buying_power -= trade_cost;
        user_entries[taker].cash_held += trade_cost;
        user_entries[taker].buying_power += trade_cost;

        user_entries[maker].amount_held[trade.asset] += trade.volume;
        user_entries[maker].selling_power[trade.asset] += trade.volume;
        user_entries[taker].amount_held[trade.asset] -= trade.volume;
        user_entries[taker].selling_power[trade.asset] -= trade.volume;
        break;
    }
  }

  [[nodiscard]] OrderResult place_order(order_t& order) {
    std::optional<std::string_view> err = validate_order(order);
    if (err.has_value()) {
      return {.error = err, .trades = {}, .unmatched = {}};
    }
    order.id = order_id++;
    OrderResult result{};
    {
      result = match_order(order);
      if (result.unmatched.has_value()) {
        add_order(result.unmatched.value());
      }
    }
    for (const auto& trade : result.trades) {
      execute_trade(order, trade);
    }
    return result;
  }

  [[nodiscard]] std::optional<order_t> cancel_order(asset_t asset, side_t side,
                                                    order_id_t order_id) {
    auto order = remove((side == BID ? bids : asks)[asset], order_id, side);
    if (!order.has_value()) {
      return std::nullopt;
    }
    switch (side) {
      case BID:
        user_entries[order->user].buying_power += order->price * order->volume;
        break;
      case ASK:
        user_entries[order->user].selling_power[asset] += order->volume;
        break;
    }
    return order;
  }
};
