#include "game.h"

namespace Checkers::NotationEncoder {

std::string encode_move(const FullPlay &play) {
  std::string encoded_move = "";
  std::optional<FieldCoordinates> last_position = std::nullopt;
  auto reference_board = CheckersGame::lazy_get_reference_board();

  for (auto &move : play) {
    auto [board_move, move_type] = move;
    auto [start_field, target_field] = board_move;
    int start_field_index = reference_board[start_field.first][start_field.second];
    int target_field_index = reference_board[target_field.first][target_field.second];

    if (!last_position.has_value()) {
      auto [_row, _col] = start_field;
      int last_field_index = reference_board[_row][_col];
      encoded_move += std::to_string(last_field_index);
    }

    switch (move_type) {
      case MoveType::BLACK_MOVE:
      case MoveType::WHITE_MOVE: {
        encoded_move += "-" + std::to_string(target_field_index);
        return encoded_move;
      }
      case MoveType::WHITE_CAPTURE:
      case MoveType::BLACK_CAPTURE: {
        encoded_move += "x" + std::to_string(target_field_index);
        break;
      }
    }

    last_position = target_field;
  }
  return encoded_move;
}

std::vector<Token> &tokenize_move(std::string const &move) {
  std::vector<Token> *tokens = new std::vector<Token>();
  TokenId current_token = TokenId::START;

  std::string buffer = "";
  for (auto &character : move) {
    switch (character) {
      case CAPTURE_SYMBOL: {
        tokens->push_back({current_token, buffer});
        current_token = TokenId::CAPTURE;
        buffer = "";
        break;
      };
      case MOVE_SYMBOL: {
        tokens->push_back({current_token, buffer});
        current_token = TokenId::MOVE;
        buffer = "";
        break;
      };
      default: {
        if (isdigit(character)) {
          if (current_token != TokenId::POSITION_INDEX) {
            tokens->push_back({current_token, buffer});
            current_token = TokenId::POSITION_INDEX;
            buffer = "";
          }
          buffer += character;
        } else {
          throw std::invalid_argument("Invalid character in move string");
        }
      };
    };

    tokens->push_back({current_token, buffer});
    tokens->push_back({TokenId::END, ""});
    return *tokens;
}

FullPlay decode_move(std::string const &move) {
  std::vector<Token> tokens;
  try {
    tokens = tokenize_move(move);
  } catch (std::invalid_argument &e) {
    return {std::nullopt, ParseError::INVALID_TOKEN};
  }

  FullPlay *decoded_play = new FullPlay();
  TokenId current_token = TokenId::START;
  for (const auto &[token_id, value] : tokens) {
    if (token_id == TokenId::END) {
      break;
    }
    if (token_id == TokenId::POSITION_INDEX) {
      int field_index = std::stoi(value);
    }
  }

}


}; // namespace Checkers::NotationEncoder
