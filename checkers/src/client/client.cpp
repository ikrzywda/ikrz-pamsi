#include "client.h"

namespace Checkers::Client {
ClientConfig::ClientConfig(int argc, char **argv) {
  if (argc != 7) {
    throw std::invalid_argument("Invalid number of arguments");
  }

  if (argv[1] == std::string("GUI")) {
    game_type = GameType::GUI;
  } else if (argv[1] == std::string("NET")) {
    game_type = GameType::NET;
  } else {
    throw std::invalid_argument("Invalid game type");
  }

  if (argv[2] == std::string("WHITE")) {
    player = Player::WHITE;
  } else if (argv[2] == std::string("BLACK")) {
    player = Player::BLACK;
  } else {
    throw std::invalid_argument("Invalid player");
  }

  minimax_depth = std::stoi(argv[3]);
  rand_seed = std::stoi(argv[4]);
  hostname = argv[5];
  port = std::stoi(argv[6]);
  process_name = argv[0];
}
}; // namespace Checkers::Client
