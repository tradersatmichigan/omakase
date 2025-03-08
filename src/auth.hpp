#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include <unordered_map>

#include "exchange.hpp"
#include "types.hpp"

namespace auth {
//NOLINTNEXTLINE (unused-function)
static std::optional<std::pair<user_t, uint32_t>> handle_register(
    const std::string& username, exchange_t& exchange) {
  if (user_ids.contains(username)) {
    return std::nullopt;
  }
  user_t user_id = exchange.register_user();
  usernames[user_id] = username;
  user_ids[username] = user_id;
  user_secrets[username] = dist(re);
  return std::make_pair(user_id, user_secrets[username]);
}

//NOLINTNEXTLINE (unused-function)
static std::optional<user_t> sign_in(const std::string& username,
                                     uint32_t secret) {
  if (!user_secrets.contains(username) || user_secrets[username] != secret) {
    return std::nullopt;
  }
  return user_ids[username];
}

};  // namespace auth
