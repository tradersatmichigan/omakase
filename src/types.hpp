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

static constexpr std::array<price_t, NUM_ASSETS> ASSET_VALUES = {
    100, 100, 200, 200, 300, 400,
};

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

struct OrderResult {
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
