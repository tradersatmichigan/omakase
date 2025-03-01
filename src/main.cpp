#include <cstdint>

#include <glaze/glaze.hpp>
#include <iostream>

#include "exchange.hpp"
#include "models.hpp"

int main() {
  exchange_t exchange;
  user_t user0 = exchange.register_user();
  user_t user1 = exchange.register_user();
  // std::cout << static_cast<uint32_t>(user0) << '\n';
  order_t order0 = {.id = 0,
                    .price = 100,
                    .volume = 1,
                    .user = user0,
                    .asset = CALIFORNIA_ROLL,
                    .side = ASK};
  // std::cout << glz::write_json(order).value_or("Error encoding JSON") << '\n';
  exchange.place_order(order0);
  order_t order1 = {.id = 0,
                    .price = 100,
                    .volume = 1,
                    .user = user1,
                    .asset = CALIFORNIA_ROLL,
                    .side = BID};
  OrderResult result = exchange.place_order(order1);
  // std::cout
  //     << glz::write_json(exchange.user_entries).value_or("Error encoding JSON")
  //     << '\n';
  // std::cout << glz::write_json(exchange.bids[order0.asset])
  //                  .value_or("Error encoding JSON")
  //           << '\n';
  std::cout << glz::write_json(result).value_or("Error encoding JSON") << '\n';
}
