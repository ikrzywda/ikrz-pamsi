#pragma once

#include "../include/checkers.h"

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

namespace Checkers::Client {

enum class GameType { GUI, NET };

enum class GameInstanceError { INVALID_MOVE, INVALID_COMMAND, GAME_OVER };
using AIMove = std::pair<std::optional<NotationTranslation::EncodedMove>,
                         std::optional<GameInstanceError>>;

struct ClientConfig {
  std::string process_name;
  GameType game_type;
  Player player;
  int minimax_depth;
  std::string hostname;
  unsigned int rand_seed;
  int port;

  ClientConfig(int argc, char **argv);
};

struct GameInstance {
  CheckersGame game_instance;

  std::optional<GameInstanceError> make_move(std::string move_code);
  AIMove play_ai_turn(Player player, int depth);
};

}; // namespace Checkers::Client

namespace Checkers::Client::Net {
std::optional<int> create_socket(ClientConfig &client_config);

struct Client {
  static void run(ClientConfig &client_config);

private:
  static Player get_player_from_turn_counter(int turn_counter);

  Client(ClientConfig &client_config);
  GameInstance game_instance;
  Player player;
  int socket_fd;

  std::string read_opponent_move();
  void send_move(std::string move_code);
};

}; // namespace Checkers::Client::Net
