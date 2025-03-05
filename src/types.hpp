#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

static constexpr int NUM_ASSETS = 6;
static constexpr int MAX_USERS = 100;

using user_t = uint8_t;
using order_id_t = uint32_t;
using price_t = uint32_t;
using volume_t = uint32_t;

enum asset_t : uint8_t {
  CALIFORNIA_ROLL = 0,
  SPICY_TUNA_ROLL = 1,
  SALMON_NIGIRI = 2,
  EEL_NIGIRI = 3,
  WHITE_TUNA_SASHIMI = 4,
  UNI_NIGIRI = 5,
};

static constexpr std::array<asset_t, NUM_ASSETS> ASSETS = {
    CALIFORNIA_ROLL, SPICY_TUNA_ROLL,    SALMON_NIGIRI,
    EEL_NIGIRI,      WHITE_TUNA_SASHIMI, UNI_NIGIRI,
};

static constexpr std::array<asset_t, 4> PLATE_1 = {
    CALIFORNIA_ROLL,
    SALMON_NIGIRI,
    WHITE_TUNA_SASHIMI,
    UNI_NIGIRI,
};

static constexpr std::array<asset_t, 4> PLATE_2 = {
    SPICY_TUNA_ROLL,
    EEL_NIGIRI,
    WHITE_TUNA_SASHIMI,
    UNI_NIGIRI,
};

static constexpr std::array<price_t, NUM_ASSETS> ASSET_VALUES = {
    100, 100, 200, 200, 300, 400,
};
static constexpr price_t PLATE_1_VALUE = 1500;
static constexpr price_t PLATE_2_VALUE = 2000;

static constexpr std::array<volume_t, NUM_ASSETS> STARTING_AMOUNTS = {
    400, 200, 200, 100, 70, 50};

static constexpr price_t STARTING_PORTFOLIO_VALUE = 200'000;

static constexpr std::array<volume_t, NUM_ASSETS> make_starting_cash() {
  std::array<volume_t, NUM_ASSETS> res = {};
  for (uint8_t asset = CALIFORNIA_ROLL; asset < NUM_ASSETS; ++asset) {
    res[asset] = STARTING_PORTFOLIO_VALUE -
                 ASSET_VALUES[asset] * STARTING_AMOUNTS[asset];
  }
  return res;
}

static constexpr std::array<volume_t, NUM_ASSETS> STARTING_CASH =
    make_starting_cash();

enum side_t : uint8_t {
  BID = 0,
  ASK = 1,
};

struct trade_t {
  order_id_t order_id;
  price_t price;
  volume_t volume;
  user_t buyer;
  user_t seller;
  asset_t asset;
};

struct order_t {
  order_id_t id;
  price_t price;
  volume_t volume;
  user_t user;
  asset_t asset;
  side_t side;
};

struct cancel_t {
  asset_t asset;
  side_t side;
  order_id_t order_id;
};

struct order_result_t {
  std::optional<std::string_view> error;
  std::vector<trade_t> trades;
  std::optional<order_t> unmatched;
};

struct user_entry_t {
  price_t cash_held;
  price_t buying_power;
  std::array<volume_t, NUM_ASSETS> amount_held;
  std::array<volume_t, NUM_ASSETS> selling_power;
};

struct register_response_t {
  std::optional<std::string> error;
  std::optional<user_t> id;
  std::optional<uint32_t> secret;
};

struct state_response_t {
  std::optional<std::string> error;
  std::optional<user_entry_t> user_entry;
  std::optional<std::array<std::vector<order_t>, NUM_ASSETS>> bids;
  std::optional<std::array<std::vector<order_t>, NUM_ASSETS>> asks;
};

enum message_t : uint8_t {
  ORDER = 0,
  CANCEL = 1,
};

struct incoming_message_t {
  // ORDER or CANCEL
  message_t type;
  // ORDER
  std::optional<order_t> order;
  std::optional<cancel_t> cancel;
};

struct outgoing_message_t {
  std::optional<std::string> error;
  std::optional<message_t> type;
  std::optional<order_result_t> order_result;
  std::optional<order_t> cancelled;
};

struct leaderboard_entry {
  user_t id;
  uint32_t rank;
  std::string username;
  price_t value;
};
