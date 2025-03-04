#include <iostream>
#include <string>
#include <thread>

#include <glaze/glaze.hpp>
#include "App.h"
#include "glaze/json/write.hpp"

#include "types.hpp"

// API setup
constexpr std::string EXAMPLE_API_ENDPOINT = "/api/greeter";
constexpr int PORT = 3000;

// per-socket information, if we need it
struct SocketData {};

// default "topic" that websockets can subscribe to
constexpr std::string DEFAULT_TOPIC = "default";

constexpr int IDLE_TIMEOUT = 10;

void get_state(uWS::HttpResponse<true>* res, uWS::HttpRequest* req) {
  // get a header value
  // NOTE: we will probably use headers for passwordless, Kahoot-esque auth
  // system
  std::cout << "Get a header value:\n";
  std::cout << "user-agent: " << req->getHeader("user-agent") << "\n\n";

  std::cout << "Iterate through headers:\n";
  for (auto iter = req->begin(); iter != req->end(); ++iter) {
    std::cout << iter.ptr->key << ": " << iter.ptr->value << "\n";
  }
  // send a response
  res->write("Hello, world!\n");
  // close the request
  res->end();
}

// us_listen_socket_t* socket = nullptr;
us_listen_socket_t* api_socket = nullptr;

void run_api() {
  uWS::SSLApp app = uWS::SSLApp();
  app.get(EXAMPLE_API_ENDPOINT, get_state);
  app.listen(PORT, [](us_listen_socket_t* listen_socket) {
    if (listen_socket) {
      api_socket = listen_socket;
      std::cout << "Listening on port " << PORT << "\n\n";
    }
  });
  app.ws<SocketData>(
      "/ws/broadcast",
      {
          .idleTimeout = IDLE_TIMEOUT,
          .open =
              [](uWS::WebSocket<true, true, SocketData>* ws) {
                // subscribe all incoming connections to default topic
                // (i.e., broadcast group)
                ws->subscribe(DEFAULT_TOPIC);
              },
          .message =
              [&app](uWS::WebSocket<true, true, SocketData>* ws,
                     std::string_view message, uWS::OpCode op_code) {
                // send this websocket a message back
                ws->send("hello there\n");
                // publish message to all connections
                app.publish(DEFAULT_TOPIC, message, op_code);
              },
      });
  app.run();
}

int main() {
  std::thread api_thread(run_api);

  std::cout << "Type \"quit\" to quit: \n";
  std::string input;
  while (std::cin >> input && input != "quit") {}

  // close the socket when we want to shut down the server, allowing us to join
  if (api_socket != nullptr) {
    us_listen_socket_close(0, api_socket);
  }
  api_thread.join();

  // some JSON serialization/deserialization
  std::string json_str =
      R"({"id": 0, "price": 10, "volume": 1, "user": 0, "asset": 6, "side": 0})";
  order_t order{};
  auto ec = glz::read_json(order, json_str);
  if (ec) {
    // handle error
    std::cout << ec.custom_error_message << '\n';
  } else {
    std::cout << glz::write_json(order).value_or("Error encoding JSON") << '\n';
  }
}
