#include "game.h"

namespace Checkers::NotationEncoder {

std::string encode_play(const FullPlay const& play) {
  std::vector<std::string> encoded_moves;
  for (auto &move : play) {
    auto [board_move, move_type] = move;
    std::string encoded_move = "";

    if (encoded_moves.size() == 0) { 
      encoded_move += std::to_string(board_move.first.first);
      encoded_move += std::to_string(board_move.first.second);
    }

    encoded_move += move_type == MoveType::WHITE_CAPTURE ? "x" : "-";
    encoded_move += std::to_string(board_move.second.first);
    encoded_move += std::to_string(board_move.second.second);
    encoded_moves.push_back(encoded_move);
  }
  return std::accumulate(encoded_moves.begin(), encoded_moves.end(), std::string(""));
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
    }
    case MOVE_SYMBOL: {
      tokens->push_back({current_token, buffer});
      current_token = TokenId::MOVE;
      buffer = "";
      break;
    }
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
    }
    }

    tokens->push_back({current_token, buffer});
    tokens->push_back({TokenId::END, ""});
    return *tokens;
  }
}

MoveDecoder::MoveDecoder(std::string const &move) {
  this->move = move;
  this->tokens = tokenize_move(move);
  this->current_token_index = 0;
}

Token MoveDecoder::get_next_token() {
  return this->tokens[++this->current_token_index];
}

Token MoveDecoder::get_current_token() {
  return this->tokens[this->current_token_index];
}

bool MoveDecoder::has_next_token() {
  return this->current_token_index < this->tokens.size();
}

ParseResult<FieldCoordinates> MoveDecoder::decode_position() {
  const auto &[token_id, value] = this->get_current_token();
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
  const auto &[token_id, _] = this->get_current_token();
  if (token_id == TokenId::CAPTURE) {
    return ParseResult<MoveType>({MoveType::WHITE_CAPTURE, std::nullopt});
  } else if (token_id == TokenId::MOVE) {
    return ParseResult<MoveType>({MoveType::WHITE_MOVE, std::nullopt});
  } else {
    return ParseResult<MoveType>({std::nullopt, ParseError::INVALID_MOVE});
  }
}

ParseResult<FullPlay> MoveDecoder::decode_play() {
  FullPlay *decoded_play = new FullPlay();

  while (this->has_next_token()) {
    if (this->get_current_token().first == TokenId::START) {
      this->get_next_token();
    }

    ParseResult<FieldCoordinates> starting_position_result =
        this->decode_position();
    if (starting_position_result.second.has_value()) {
      return ParseResult<FullPlay>(
          {std::nullopt, starting_position_result.second});
    }

    if (!this->has_next_token()) {
      return ParseResult<FullPlay>({std::nullopt, ParseError::TOO_MANY_MOVES});
    }
    this->get_next_token();

    ParseResult<MoveType> move_type_result = this->decode_move_type();
    if (move_type_result.second.has_value()) {
      return ParseResult<FullPlay>({std::nullopt, move_type_result.second});
    }

    if (!this->has_next_token()) {
      return ParseResult<FullPlay>({std::nullopt, ParseError::TOO_MANY_MOVES});
    }
    this->get_next_token();

    ParseResult<FieldCoordinates> target_position_result =
        this->decode_position();
    if (target_position_result.second.has_value()) {
      return ParseResult<FullPlay>(
          {std::nullopt, target_position_result.second});
    }

    BoardMove move = {starting_position_result.first.value(),
                      target_position_result.first.value()};
    decoded_play->push_back({move, move_type_result.first.value()});
  }
  return ParseResult<FullPlay>({*decoded_play, std::nullopt});
}

ParseResult<FullPlay> MoveDecoder::decode_move(std::string const &move) {
  MoveDecoder decoder = MoveDecoder(move);
  return decoder.decode_play();
};
