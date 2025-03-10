#include <cassert>
#include <exchange.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include <glaze/glaze.hpp>

namespace logging {
static const std::string USERS_FILENAME = "users.log";
static const std::string MESSAGES_FILENAME = "messages.log";
//NOLINTBEGIN (cppcoreguidelines-interfaces-global-init)
static std::ofstream users_file(USERS_FILENAME, std::ios::app);
static std::ofstream messages_file(MESSAGES_FILENAME, std::ios::app);
//NOLINTEND (cppcoreguidelines-interfaces-global-init)

//NOLINTBEGIN (unused-function)
static void log_user(const std::string& username) {
  if (users_file.is_open()) {
    users_file << username << '\n';
    users_file.flush();
  } else {
    std::cerr << "Failed to open " << USERS_FILENAME << '\n';
    assert(false);
  }
}

static void log_message(const std::string_view& message) {
  if (messages_file.is_open()) {
    messages_file << message << '\n';
    messages_file.flush();
  } else {
    std::cerr << "Failed to open " << MESSAGES_FILENAME << '\n';
    assert(false);
  }
}
//NOLINTEND (unused-function)
};  // namespace logging
