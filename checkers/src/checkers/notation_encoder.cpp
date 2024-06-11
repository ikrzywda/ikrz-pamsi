#include "game.h"

namespace Checkers::NotationTranslation {

EncodedMove encode_play(Turn const &play) {
  std::vector<std::string> encoded_moves;
  auto reference_board = CheckersGame::lazy_get_reference_board();

  for (auto &move : play) {
    auto [board_move, move_type] = move;
    int start_field_index =
        reference_board[board_move.first.first][board_move.first.second];
    if (start_field_index == 0) {
      throw std::invalid_argument("Invalid starting position");
    }
    int target_field_index =
        reference_board[board_move.second.first][board_move.second.second];
    if (target_field_index == 0) {
      throw std::invalid_argument("Invalid target position");
    }

    std::string encoded_move = "";

    if (encoded_moves.size() == 0) {
      encoded_move += std::to_string(start_field_index);
    }

    encoded_move += move_type == MoveType::WHITE_CAPTURE ? "x" : "-";
    encoded_move += std::to_string(target_field_index);
    encoded_moves.push_back(encoded_move);
  }
  std::string res = std::accumulate(encoded_moves.begin(), encoded_moves.end(),
                                    std::string(""));
  return res;
}

std::vector<Token> tokenize_move(std::string const &move) {
  std::vector<Token> tokens = std::vector<Token>();
  TokenId current_token = TokenId::START;

  std::string buffer = "";
  for (auto &character : move) {
    switch (character) {
    case CAPTURE_SYMBOL: {
      tokens.push_back({current_token, buffer});
      current_token = TokenId::CAPTURE;
      buffer = "";
      break;
    }
    case MOVE_SYMBOL: {
      tokens.push_back({current_token, buffer});
      current_token = TokenId::MOVE;
      buffer = "";
      break;
    }
    default: {
      if (isdigit(character)) {
        if (current_token != TokenId::POSITION_INDEX) {
          tokens.push_back({current_token, buffer});
          current_token = TokenId::POSITION_INDEX;
          buffer = "";
        }
        buffer += character;
      } else {
        throw std::invalid_argument("Invalid character in move string");
      }
    }
    }
  }
  tokens.push_back({current_token, buffer});
  tokens.push_back({TokenId::END, ""});
  return tokens;
}

MoveDecoder::MoveDecoder(std::string const &move) {
  this->move = move;
  tokens = tokenize_move(move);
  current_token_index = 0;
}

Token MoveDecoder::get_next_token() {
  current_token_index++;
  return tokens[current_token_index];
}

Token MoveDecoder::peek_next_token() { return tokens[current_token_index + 1]; }

Token MoveDecoder::get_current_token() { return tokens[current_token_index]; }

bool MoveDecoder::has_next_token() {
  return (unsigned int)current_token_index < tokens.size();
}

ParseResult<FieldCoordinates> MoveDecoder::decode_position() {
  const auto &[token_id, value] = get_current_token();
  if (token_id != TokenId::POSITION_INDEX) {
    return ParseResult<FieldCoordinates>(
        {std::nullopt, ParseError::INVALID_POSITION});
  }

  int starting_field_index = std::stoi(value);
  std::optional<FieldCoordinates> starting_position =
      CheckersGame::translate_field_index(starting_field_index);

  if (!starting_position.has_value()) {
    return ParseResult<FieldCoordinates>(
        {std::nullopt, ParseError::INVALID_POSITION});
  }

  return ParseResult<FieldCoordinates>(
      {starting_position.value(), std::nullopt});
}

ParseResult<MoveType> MoveDecoder::decode_move_type() {
  const auto &[token_id, _] = get_current_token();
  if (token_id == TokenId::CAPTURE) {
    return ParseResult<MoveType>({MoveType::WHITE_CAPTURE, std::nullopt});
  } else if (token_id == TokenId::MOVE) {
    return ParseResult<MoveType>({MoveType::WHITE_MOVE, std::nullopt});
  } else {
    return ParseResult<MoveType>({std::nullopt, ParseError::INVALID_MOVE});
  }
}

ParseResult<Turn> MoveDecoder::decode_play() {
  Turn *decoded_play = new Turn();

  while (has_next_token()) {
    if (get_current_token().first == TokenId::END) {
      break;
    }
    if (get_current_token().first == TokenId::START) {
      get_next_token();
    }

    ParseResult<FieldCoordinates> starting_position_result = decode_position();

    if (starting_position_result.second.has_value()) {
      return ParseResult<Turn>({std::nullopt, starting_position_result.second});
    }

    if (!has_next_token()) {
      return ParseResult<Turn>({std::nullopt, ParseError::TOO_MANY_MOVES});
    }
    get_next_token();

    ParseResult<MoveType> move_type_result = decode_move_type();
    if (move_type_result.second.has_value()) {
      return ParseResult<Turn>({std::nullopt, move_type_result.second});
    }

    if (!has_next_token()) {
      return ParseResult<Turn>({std::nullopt, ParseError::TOO_MANY_MOVES});
    }
    get_next_token();

    ParseResult<FieldCoordinates> target_position_result = decode_position();
    if (target_position_result.second.has_value()) {
      return ParseResult<Turn>({std::nullopt, target_position_result.second});
    }

    BoardMove move = {starting_position_result.first.value(),
                      target_position_result.first.value()};
    decoded_play->push_back({move, move_type_result.first.value()});

    if (has_next_token()) {
      auto [token_id, _] = peek_next_token();
      if (token_id == TokenId::END) {
        get_next_token();
      }
    }
  }
  return ParseResult<Turn>({*decoded_play, std::nullopt});
}

ParseResult<Turn> MoveDecoder::decode_move(std::string const &move) {
  MoveDecoder decoder = MoveDecoder(move);
  return decoder.decode_play();
};
}; // namespace Checkers::NotationTranslation
