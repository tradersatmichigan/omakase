#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

#include "heap.hpp"
#include "types.hpp"

namespace auth {
static std::unordered_map<user_t, std::string> usernames;
static std::unordered_map<std::string, user_t> user_ids;
static std::unordered_map<std::string, uint32_t> user_secrets;

static std::default_random_engine re;
static std::uniform_int_distribution<uint32_t> dist;
}  // namespace auth

struct exchange_t {
  user_t num_users = 0;
  uint8_t next_asset = CALIFORNIA_ROLL;
  order_id_t order_id = 0;
  std::array<user_entry_t, MAX_USERS> user_entries = {};
  std::array<std::vector<order_t>, NUM_ASSETS> bids = {};
  std::array<std::vector<order_t>, NUM_ASSETS> asks = {};

  [[nodiscard]] price_t get_portfolio_value(user_t user) const {
    if (user >= num_users) {
      return 0;
    }
    auto user_entry = user_entries[user];
    volume_t plate_2_count = user_entry.amount_held[*std::ranges::min_element(
        PLATE_2, [&user_entry](asset_t lhs, asset_t rhs) {
          return user_entry.amount_held[lhs] < user_entry.amount_held[rhs];
        })];
    std::ranges::for_each(PLATE_2, [&user_entry, plate_2_count](asset_t asset) {
      user_entry.amount_held[asset] -= plate_2_count;
    });
    volume_t plate_1_count = user_entry.amount_held[*std::ranges::min_element(
        PLATE_1, [&user_entry](asset_t lhs, asset_t rhs) {
          return user_entry.amount_held[lhs] < user_entry.amount_held[rhs];
        })];
    std::ranges::for_each(PLATE_1, [&user_entry, plate_1_count](asset_t asset) {
      user_entry.amount_held[asset] -= plate_1_count;
    });
    price_t value = user_entry.cash_held;
    std::ranges::for_each(ASSETS, [&user_entry, &value](asset_t asset) {
      value += ASSET_VALUES[asset] * user_entry.amount_held[asset];
    });
    if (plate_1_count > 0 || plate_2_count > 0) {
      std::cout << "plate_1_count: " << plate_1_count
                << "plate_2_count: " << plate_2_count << '\n';
    }
    value += PLATE_1_VALUE * plate_1_count;
    value += PLATE_2_VALUE * plate_2_count;
    return value;
  }

  [[nodiscard]] std::vector<leaderboard_entry> get_leaderboard() const {
    std::vector<leaderboard_entry> res;
    res.reserve(num_users);
    for (user_t user = 0; user < num_users; ++user) {
      res.emplace_back(user, 0, auth::usernames[user],
                       get_portfolio_value(user));
    }
    std::ranges::sort(
        res, [](const leaderboard_entry& lhs, const leaderboard_entry& rhs) {
          return lhs.value > rhs.value;
        });
    for (uint32_t rank = 1; rank <= num_users; ++rank) {
      res[rank - 1].rank = rank;
    }
    return res;
  }

  [[nodiscard]] state_response_t get_state(user_t user) const {
    if (user >= num_users) {
      return {
          .error = "Invalid user id: " + std::to_string(user),
          .user_entry = std::nullopt,
          .bids = std::nullopt,
          .asks = std::nullopt,
      };
    }
    return {
        .error = std::nullopt,
        .user_entry = user_entries[user],
        .bids = bids,
        .asks = asks,
    };
  }

  void verify_state() const {
    for (user_t user = 0; user < num_users; ++user) {
      auto& user_entry = user_entries[user];
      price_t expected_buying_power = user_entry.cash_held;
      for (const auto& heap : bids) {
        for (const auto& order : heap) {
          if (order.user == user) {
            expected_buying_power -= order.price * order.volume;
          }
        }
        assert(std::ranges::is_heap(heap, heap::CMP(BID)));
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

        assert(std::ranges::is_heap(asks[asset], heap::CMP(ASK)));
        assert(expected_selling_power == user_entry.selling_power[asset]);
      }
    }
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

  [[nodiscard]] std::optional<std::string> validate_order(
      const order_t& order) const {
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
          return "Insufficient selling power for asset " + SYMBOLS[order.asset];
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
    heap::push(orders, order);
  }

  [[nodiscard]] order_result_t match_order(order_t& order) {
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
        heap::pop(opposing_orders, other.side);
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

  [[nodiscard]] order_result_t place_order(order_t& order) {
    std::optional<std::string> err = validate_order(order);
    if (err.has_value()) {
      return {.error = err, .trades = {}, .unmatched = {}};
    }
    order.id = order_id++;
    order_result_t result{};
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
    auto order =
        heap::remove((side == BID ? bids : asks)[asset], order_id, side);
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
