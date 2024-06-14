#include "client.h"

namespace Checkers::Client {

std::optional<GameInstanceError>
GameInstance::make_move(std::string move_code) {
  auto [play, parse_error] =
      NotationTranslation::MoveDecoder::decode_move(move_code);
  if (parse_error.has_value()) {
    return GameInstanceError::INVALID_MOVE;
  }

  if (!play.has_value()) {
    throw std::runtime_error("Unexpected error");
  }

  if (play.value().empty()) {
    return GameInstanceError::INVALID_MOVE;
  }

  for (const auto &[move, _] : play.value()) {
    const auto [start_field, target_field] = move;
    const auto [start_row, start_col] = start_field;
    const auto [target_row, target_col] = target_field;
    const auto start_field_index =
        Utilities::get_field_index(start_row, start_col);
    const auto target_field_index =
        Utilities::get_field_index(target_row, target_col);
    game_instance.make_move(start_field, target_field);
  }

  return std::nullopt;
}

AIMove GameInstance::play_ai_turn(Player player, int depth) {
  auto turn = game_instance.play_ai_turn(depth, player);

  if (!turn.has_value()) {
    return {std::nullopt, GameInstanceError::GAME_OVER};
  }

  game_instance.print_board();

  auto encoded_move = NotationTranslation::encode_play(turn.value());
  return AIMove{encoded_move, std::nullopt};
}
} // namespace Checkers::Client
