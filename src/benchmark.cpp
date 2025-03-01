#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include "exchange.hpp"
#include "models.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::mutex output_mutex;

exchange_t exchange;

static constexpr size_t NUM_THREADS = 6;
static_assert(0 < NUM_THREADS && NUM_THREADS <= NUM_ASSETS);

static constexpr int NUM_USERS = 100;
static constexpr int ORDERS_PER_ASSET = 10'000'000;

static constexpr price_t BENCHMARK_CASH = 4'000'000'000;
static constexpr std::array<volume_t, NUM_ASSETS> BENCHMARK_ASSETS = {
    100'000'000, 100'000'000, 100'000'000,
    100'000'000, 100'000'000, 100'000'000};
static constexpr price_t MIN_PRICE = 1;
static constexpr price_t MAX_PRICE = 2000;
static constexpr price_t MIN_VOLUME = 1;
static constexpr price_t MAX_VOLUME = 20;

std::vector<user_t> register_users() {
  std::vector<user_t> res;
  res.reserve(NUM_USERS);
  for (int i = 0; i < NUM_USERS; ++i) {
    res.push_back(exchange.register_user(BENCHMARK_CASH, BENCHMARK_ASSETS));
  }
  return res;
}

std::vector<order_t> generate_orders(asset_t asset,
                                     const std::vector<user_t>& users) {
  std::default_random_engine e1(42);
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

auto benchmark(asset_t asset, const std::vector<user_t>& users) -> void {
  std::vector<order_t> orders = generate_orders(asset, users);

  auto t_start = std::chrono::high_resolution_clock::now();
  for (auto& order : orders) {
    auto res = exchange.place_order(order);
    if (res.error.has_value()) {
      std::scoped_lock lock(output_mutex);
      std::cout << res.error.value() << '\n';
    }
  }
  auto t_end = std::chrono::high_resolution_clock::now();

  {
    std::scoped_lock lock(output_mutex);
    std::cout << "Placing orders took: "
              << std::chrono::duration<double, std::milli>(t_end - t_start)
              << '\n';
  }
}

auto main() -> int {
  std::vector<std::thread*> threads(NUM_THREADS);

  std::vector<user_t> users = register_users();

  for (size_t i = 0; i < NUM_THREADS; ++i) {
    threads[i] = new std::thread(
        [i, users]() { benchmark(static_cast<asset_t>(i), users); });
  }

  std::ranges::for_each(threads, [](std::thread* t) { t->join(); });

  return 0;
}
