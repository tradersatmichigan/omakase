#include <charconv>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <glaze/glaze.hpp>
#include "App.h"

#include "auth.hpp"
#include "exchange.hpp"
#include "log.hpp"
#include "types.hpp"

// API endpoints
const std::string STATE_URL = "/api/state";
const std::string REGISTER_URL = "/api/register";
const std::string SIGN_IN_URL = "/api/sign_in";
const std::string LEADERBOARD_URL = "/api/leaderboard";

constexpr int PORT = 3000;

// per-socket information, if we need it
struct socket_data {};

// default "topic" that websockets can subscribe to
constexpr std::string DEFAULT_TOPIC = "default";

constexpr int IDLE_TIMEOUT = 10;
const std::string JSON_ENCODE_ERROR = R"({"error": "Error encoding JSON."})";

exchange_t exchange;

void send_response(uWS::HttpResponse<true>* res, const auto& response) {
  res->end(glz::write_json(response).value_or(JSON_ENCODE_ERROR));
}

void get_state(uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
  user_t user{0};
  auto user_str = req->getQuery("user");
  if (user_str.empty()) {
    send_response(res, state_response_t{
                           .error = "Must include user header",
                           .user_entry = std::nullopt,
                           .bids = std::nullopt,
                           .asks = std::nullopt,
                       });
    return;
  }
  auto parse_result =
      std::from_chars(user_str.data(), user_str.data() + user_str.size(), user);
  if (parse_result.ec == std::errc::invalid_argument) {
    send_response(res, state_response_t{
                           .error = "User id parse error: " +
                                    std::string(parse_result.ptr),
                           .user_entry = std::nullopt,
                           .bids = std::nullopt,
                           .asks = std::nullopt,
                       });
    return;
  }
  send_response(res, exchange.get_state(user));
}

void get_leaderboard(uWS::HttpResponse<true>* res,
                     uWS::HttpRequest* /* req */) {
  send_response(res, exchange.get_leaderboard());
}

void handle_register(uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
  auto username = std::string(req->getQuery("username"));
  auto info = auth::handle_register(username, exchange);
  if (info.has_value()) {
    log::log_user(username, info->first, info->second);
    send_response(res, register_response_t{.error = std::nullopt,
                                           .id = info->first,
                                           .secret = info->second});
  } else {
    send_response(
        res, register_response_t{.error = "User with username \"" + username +
                                          "\" already exists.",
                                 .id = std::nullopt,
                                 .secret = std::nullopt});
  }
}

void handle_sign_in(uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
  auto username = std::string(req->getQuery("username"));
  auto secret_str = req->getQuery("secret");
  uint32_t secret{};
  auto parse_result = std::from_chars(
      secret_str.data(), secret_str.data() + secret_str.size(), secret);
  if (parse_result.ec == std::errc::invalid_argument) {
    send_response(res, register_response_t{.error = "Secret parse error",
                                           .id = std::nullopt,
                                           .secret = std::nullopt});
    return;
  }
  auto id = auth::sign_in(username, secret);
  if (id.has_value()) {
    send_response(res,
                  register_response_t{
                      .error = std::nullopt, .id = id, .secret = std::nullopt});
  } else {
    send_response(res, register_response_t{.error = "Error signing in",
                                           .id = std::nullopt,
                                           .secret = std::nullopt});
  }
}

outgoing_message_t handle_message(std::string_view message) {
  incoming_message_t incoming;
  auto ec = glz::read_json(incoming, message);
  outgoing_message_t response;
  if (ec) {
    response.error = std::string(ec.custom_error_message);
    return response;
  }
  switch (incoming.type) {
    case ORDER:
      if (!incoming.order.has_value()) {
        response.error = "Must provide order field";
        break;
      }
      response.order_result = exchange.place_order(incoming.order.value());
      response.type = message_t::ORDER;
      if (response.order_result->error.has_value()) {
        response.error = response.order_result->error;
      }
      break;
    case CANCEL:
      if (!incoming.cancel.has_value()) {
        response.error = "Must provide cancel field";
        break;
      }
      response.cancelled =
          exchange.cancel_order(incoming.cancel->asset, incoming.cancel->side,
                                incoming.cancel->order_id);
      response.type = message_t::CANCEL;
      if (!response.cancelled.has_value()) {
        response.error = "Order not found";
      }
      break;
  }
  return response;
}

us_listen_socket_t* api_socket = nullptr;

void run_api() {
  uWS::SSLApp app = uWS::SSLApp();
  app.get(STATE_URL, get_state);
  app.get(LEADERBOARD_URL, get_leaderboard);
  app.post(REGISTER_URL, handle_register);
  app.post(SIGN_IN_URL, handle_sign_in);
  app.listen(PORT, [](us_listen_socket_t* listen_socket) {
    if (listen_socket) {
      api_socket = listen_socket;
    }
  });
  app.ws<socket_data>(
      "/ws",
      {
          .idleTimeout = IDLE_TIMEOUT,
          .open =
              [](uWS::WebSocket<true, true, socket_data>* ws) {
                ws->subscribe(DEFAULT_TOPIC);
              },
          .message =
              [&app](uWS::WebSocket<true, true, socket_data>* ws,
                     std::string_view message, uWS::OpCode op_code) {
                outgoing_message_t outgoing = handle_message(message);
                if (outgoing.error.has_value()) {
                  ws->send(
                      glz::write_json(outgoing).value_or(JSON_ENCODE_ERROR),
                      op_code);
                } else {
                  app.publish(
                      DEFAULT_TOPIC,
                      glz::write_json(outgoing).value_or(JSON_ENCODE_ERROR),
                      op_code);
                }
              },
      });
  app.run();
}

int main() {
  run_api();
}
