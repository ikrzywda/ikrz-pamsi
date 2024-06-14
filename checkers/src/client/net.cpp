#include "client.h"

namespace Checkers::Client::Net {
std::optional<int> create_socket(ClientConfig &client_config) {
  if (client_config.game_type == GameType::GUI) {
    return std::nullopt;
  }
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    return std::nullopt;
  }

  struct hostent *hostent_server =
      gethostbyname(client_config.hostname.c_str());
  if (hostent_server == nullptr) {
    return std::nullopt;
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  memcpy(&server_address.sin_addr, hostent_server->h_addr,
         hostent_server->h_length);
  server_address.sin_port = htons(client_config.port);

  if (connect(socket_fd, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    return std::nullopt;
  }

  return socket_fd;
}

Client::Client(ClientConfig &client_config) {
  auto _socket_fd = create_socket(client_config);
  if (!_socket_fd.has_value()) {
    throw std::runtime_error("Failed to create socket");
  }
  socket_fd = _socket_fd.value();
  game_instance = GameInstance();
  player = client_config.player;
}

std::string Client::read_opponent_move() {
  char buffer[1024];
  int n = read(socket_fd, buffer, sizeof(buffer));
  if (n < 0) {
    throw std::runtime_error("Failed to read from socket");
  }
  if (n == 0) {
    throw std::runtime_error("Connection closed by server");
  }
  buffer[n] = 0;
  return std::string(buffer);
}

void Client::send_move(std::string move_code) {
  if (write(socket_fd, move_code.c_str(), move_code.size()) < 0) {
    throw std::runtime_error("Failed to write to socket");
  }
}

Player Client::get_player_from_turn_counter(int turn_counter) {
  return turn_counter % 2 == 0 ? Player::BLACK : Player::WHITE;
}

void Client::run(ClientConfig &client_config) {
  Client client = Client(client_config);
  CheckersGame::set_random_seed(client_config.rand_seed);
  int turn_counter = 0;

  auto player_turn_predicate = [&client, &turn_counter]() {
    return client.player == Client::get_player_from_turn_counter(turn_counter);
  };

  while (true) {
    if (player_turn_predicate()) {
      auto [encoded_turn, error] = client.game_instance.play_ai_turn(
          client.player, client_config.minimax_depth);
      if (error.has_value()) {
        std::cerr << "Game over" << std::endl;
        break;
      }
      client.send_move(encoded_turn.value());
    } else {
      std::string opponent_move = client.read_opponent_move();
      std::cout << "Opponent move: " << opponent_move << std::endl;
      auto error = client.game_instance.make_move(opponent_move);
      if (error.has_value()) {
        std::cerr << "Invalid move" << std::endl;
        break;
      }
    }

    turn_counter++;
  }
}

}; // namespace Checkers::Client::Net
