#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include <glaze/glaze.hpp>
#include <tuple>

#include "glaze/json/write.hpp"
#include "types.hpp"

namespace log {
static const std::string USERS_FILENAME = "users.log";
static const std::string ORDERS_FILENAME = "orders.log";
//NOLINTBEGIN (cppcoreguidelines-interfaces-global-init)
static std::ofstream users_file(USERS_FILENAME, std::ios::app);
static std::ofstream orders_file(ORDERS_FILENAME, std::ios::app);
//NOLINTEND (cppcoreguidelines-interfaces-global-init)

//NOLINTBEGIN (unused-function)
static void log_message(const std::string& message) {
  if (users_file.is_open()) {
    users_file << message << '\n';
    users_file.flush();
  } else {
    std::cerr << "Failed to open log file!\n";
    assert(false);
  }
}

static void log_user(const std::string& username, user_t id, uint32_t secret) {
  if (users_file.is_open()) {
    users_file << glz::write_json(std::make_tuple(username, id, secret))
                      .value_or("error")
               << '\n';
    users_file.flush();
  } else {
    std::cerr << "Failed to open " << USERS_FILENAME << '\n';
    assert(false);
  }
}
//NOLINTEND (unused-function)
};  // namespace log
