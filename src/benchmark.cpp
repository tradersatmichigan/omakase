#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <ratio>
#include <thread>
#include <vector>

#include <glaze/glaze.hpp>

#include "exchange.hpp"
#include "types.hpp"

exchange_t exchange;

static constexpr bool CHECK_STATE = false;
static constexpr size_t NUM_THREADS = 1;
static_assert(0 < NUM_THREADS && NUM_THREADS <= NUM_ASSETS);

static constexpr int NUM_USERS = MAX_USERS;
static constexpr int ORDERS_PER_ASSET = 6'000'000 / NUM_THREADS;

static constexpr price_t BENCHMARK_CASH = 4'000'000'000;
static constexpr volume_t BENCHMARK_VOLUME = 100'000'000;
static constexpr std::array<volume_t, NUM_ASSETS> BENCHMARK_ASSETS = {
    BENCHMARK_VOLUME, BENCHMARK_VOLUME, BENCHMARK_VOLUME,
    BENCHMARK_VOLUME, BENCHMARK_VOLUME, BENCHMARK_VOLUME,
};
static constexpr price_t MIN_PRICE = 1;
static constexpr price_t MAX_PRICE = 2000;
static constexpr price_t MIN_VOLUME = 1;
static constexpr price_t MAX_VOLUME = 200;

std::vector<user_t> register_users() {
  std::vector<user_t> res;
  res.reserve(NUM_USERS);
  for (int i = 0; i < NUM_USERS; ++i) {
    res.push_back(exchange.register_user(BENCHMARK_CASH, BENCHMARK_ASSETS));
  }
  return res;
}

std::default_random_engine e1(42);

std::vector<order_t> generate_orders(const std::vector<user_t>& users) {
  std::uniform_int_distribution<uint8_t> asset_generator(0, NUM_ASSETS - 1);
  std::uniform_int_distribution<uint8_t> side_generator(0, 1);
  std::uniform_int_distribution<size_t> id_generator(0, users.size() - 1);
  std::uniform_int_distribution<price_t> price_generator(MIN_PRICE, MAX_PRICE);
  std::uniform_int_distribution<volume_t> volume_generator(MIN_VOLUME,
                                                           MAX_VOLUME);

  std::vector<order_t> orders;
  orders.reserve(ORDERS_PER_ASSET);

  for (size_t i = 0; i < ORDERS_PER_ASSET; ++i) {
    auto asset = static_cast<asset_t>(asset_generator(e1));
    auto side = static_cast<side_t>(side_generator(e1));
    user_t user = users[id_generator(e1)];
    price_t price = price_generator(e1);
    volume_t volume = volume_generator(e1);
    orders.emplace_back(0, price, volume, user, asset, side);
  }

  return orders;
}

std::vector<order_t> generate_orders(asset_t asset,
                                     const std::vector<user_t>& users) {
  std::uniform_int_distribution<uint32_t> side_generator(0, 1);
  std::uniform_int_distribution<size_t> id_generator(0, users.size() - 1);
  std::uniform_int_distribution<price_t> price_generator(MIN_PRICE, MAX_PRICE);
  std::uniform_int_distribution<volume_t> volume_generator(MIN_VOLUME,
                                                           MAX_VOLUME);

  std::vector<order_t> orders;
  orders.reserve(ORDERS_PER_ASSET);

  for (size_t i = 0; i < ORDERS_PER_ASSET; ++i) {
    auto side = static_cast<side_t>(side_generator(e1));
    user_t user = users[id_generator(e1)];
    price_t price = price_generator(e1);
    volume_t volume = volume_generator(e1);
    orders.emplace_back(0, price, volume, user, asset, side);
  }

  return orders;
}

auto benchmark(const std::vector<user_t>& users) -> void {
  std::vector<order_t> orders = generate_orders(users);

  std::vector<std::chrono::duration<double, std::nano>> response_times;
  response_times.reserve(orders.size());
  std::uniform_int_distribution<int> cancel_generator(1, 10);

  std::cout << "starting benchmark" << '\n';
  for (auto& order : orders) {
    // std::cout << glz::write_json(order).value_or("Error encoding JSON") << '\n';
    auto t_start = std::chrono::high_resolution_clock::now();
    auto res = exchange.place_order(order);
    auto t_end = std::chrono::high_resolution_clock::now();
    // std::cout << glz::write_json(res).value_or("Error encoding JSON") << '\n';
    if (res.unmatched.has_value() && cancel_generator(e1) == 1) {
      auto _ = exchange.cancel_order(order.asset, order.side,
                                     res.unmatched.value().id);
    }
    if constexpr (CHECK_STATE) {
      exchange.verify_state();
    }
    response_times.emplace_back(t_end - t_start);
    if (res.error.has_value()) {
      std::cout << res.error.value() << '\n';
    }
  }

  std::ranges::sort(response_times);
  std::chrono::duration<double> total_time =
      std::accumulate(response_times.begin(), response_times.end(),
                      std::chrono::duration<double, std::nano>(0.0));
  std::chrono::duration<double, std::nano> average_response_time =
      total_time / static_cast<float>(orders.size());
  std::cout << "Placing orders took: " << total_time << '\n';
  std::cout << "Average time per order: " << average_response_time << "\n";
  for (double percent = 0.1; percent >= 0.0000001; percent /= 10) {
    std::cout << percent * 100
              << "% latency: " << response_times[orders.size() * (1 - percent)]
              << '\n';
  }
}

auto main() -> int {
  std::vector<std::thread*> threads(NUM_THREADS);

  std::vector<user_t> users = register_users();
  benchmark(users);

  exchange.verify_state();
  std::cout << "PASS\n";

  return 0;
}
