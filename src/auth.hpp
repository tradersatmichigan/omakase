#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "exchange.hpp"
#include "types.hpp"

namespace auth {
static std::unordered_set<uint32_t> used_secrets;
// NOLINTBEGIN (unused-function)
static std::optional<std::pair<user_t, uint32_t>> handle_register(
    const std::string& username, exchange_t& exchange) {
  if (user_ids.contains(username)) {
    return std::nullopt;
  }
  user_t user_id = exchange.register_user();
  usernames[user_id] = username;
  user_ids[username] = user_id;
  uint32_t secret = dist(re);
  while (used_secrets.contains(secret)) {
    secret = dist(re);
  }
  used_secrets.insert(secret);
  user_secrets[username] = secret;
  return std::make_pair(user_id, user_secrets[username]);
}

static std::optional<user_t> sign_in(const std::string& username,
                                     uint32_t secret) {
  if (!user_secrets.contains(username) || user_secrets[username] != secret) {
    return std::nullopt;
  }
  return user_ids[username];
}
// NOLINTEND (unused-function)

};  // namespace auth
