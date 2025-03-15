#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include <glaze/glaze.hpp>

#include "types.hpp"

namespace logging {
static std::string users_filename;
static std::string messages_filename;
static std::string secrets_filename;
static std::string leaderboard_filename;
//NOLINTBEGIN (cppcoreguidelines-interfaces-global-init)
static std::ofstream users_file;
static std::ofstream messages_file;
static std::ofstream secrets_file;
static std::ofstream leaderboard_file;
//NOLINTEND (cppcoreguidelines-interfaces-global-init)

//NOLINTBEGIN (unused-function)
static void init(int game_number) {
  std::filesystem::path game_dir = "game" + std::to_string(game_number);

  if (std::filesystem::create_directories(game_dir)) {
    std::cout << "Directories created: " << game_dir << '\n';
  } else {
    std::cout << "Directory " << game_dir << " already exists\n";
  }

  users_filename = "game" + std::to_string(game_number) + "/users.log";
  messages_filename = "game" + std::to_string(game_number) + "/messages.log";
  secrets_filename = "game" + std::to_string(game_number) + "/secrets.log";
  leaderboard_filename =
      "game" + std::to_string(game_number) + "/leaderboard.csv";
  users_file = std::ofstream(users_filename, std::ios::app);
  messages_file = std::ofstream(messages_filename, std::ios::app);
  secrets_file = std::ofstream(secrets_filename, std::ios::app);
  leaderboard_file = std::ofstream(leaderboard_filename);
}

static void log_user(const std::string& username, user_t user_id,
                     uint32_t secret) {
  if (users_file.is_open()) {
    users_file << username << '\n';
    users_file.flush();
  } else {
    std::cerr << "Failed to open " << users_filename << '\n';
  }

  if (secrets_file.is_open()) {
    secrets_file << username << ", " << static_cast<int>(user_id) << ", "
                 << secret << '\n';
    secrets_file.flush();
  } else {
    std::cerr << "Failed to open " << secrets_filename << '\n';
  }
}

static void log_message(const std::string_view& message) {
  if (messages_file.is_open()) {
    messages_file << message << '\n';
    messages_file.flush();
  } else {
    std::cerr << "Failed to open " << messages_filename << '\n';
  }
}

static void log_leaderboard(const std::vector<leaderboard_entry>& leaderboard) {
  if (leaderboard_file.is_open()) {
    leaderboard_file << "place,name,value\n";
    for (size_t i = 0; i < leaderboard.size(); ++i) {
      const auto& entry = leaderboard[i];
      leaderboard_file << i + 1 << ',' << entry.username << ',' << entry.value
                       << '\n';
    }
    leaderboard_file.flush();
  } else {
    std::cerr << "Failed to open " << leaderboard_filename << '\n';
  }
}
//NOLINTEND (unused-function)
};  // namespace logging
