#include "game.h"

namespace Checkers {

int Minimax::evaluate_board(Board &board) {
  int score = 0;
  for (int row = 0; row < BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < BOARD_SIDE_LENGTH; ++col) {
      BoardField field = (BoardField)board[row][col];
      if (PIECE_VALUES.find(field) == PIECE_VALUES.end()) {
        continue;
      }
      score += PIECE_VALUES.at(field);
    }
  }
  return score;
}

int Minimax::minimax(CheckersGame &game, int alpha, int beta, int depth,
                     Player player) {
  if (!depth) {
    return evaluate_board(game.game_board);
  }

  auto _possible_moves = game.get_possible_moves(player);
  if (!_possible_moves.has_value()) {
    return evaluate_board(game.game_board);
  }
  auto [possible_moves, has_captures] = _possible_moves.value();

  if (possible_moves.empty()) {
    return evaluate_board(game.game_board);
  }

  if (player == Player::WHITE) {
    int max_eval = INT_MIN;
    for (auto &move : possible_moves) {
      auto [starting_position, possible_field_moves] = move;
      auto [target_positions, _has_captures] = possible_field_moves;
      if (has_captures && !_has_captures) {
        continue;
      }
      for (auto &target_position : target_positions) {
        CheckersGame new_game = game; // Copy the current game state
        new_game.make_move(starting_position, target_position);
        int eval_score =
            minimax(new_game, alpha, beta, depth - 1, Player::BLACK);
        max_eval = std::max(max_eval, eval_score);
        alpha = std::max(alpha, eval_score);
        if (beta <= alpha) {
          break; // Beta cut-off
        }
      }
      if (beta <= alpha) {
        break; // Beta cut-off
      }
    }
    return max_eval;
  } else { // Player::BLACK
    int min_eval = INT_MAX;
    for (auto &move : possible_moves) {
      auto [starting_position, possible_field_moves] = move;
      auto [target_positions, _has_captures] = possible_field_moves;
      if (has_captures && !_has_captures) {
        continue;
      }
      for (auto &target_position : target_positions) {
        CheckersGame new_game = game; // Copy the current game state
        new_game.make_move(starting_position, target_position);
        int eval_score =
            minimax(new_game, alpha, beta, depth - 1, Player::WHITE);
        min_eval = std::min(min_eval, eval_score);
        beta = std::min(beta, eval_score);
        if (beta <= alpha) {
          break; // Alpha cut-off
        }
      }
      if (beta <= alpha) {
        break; // Alpha cut-off
      }
    }
    return min_eval;
  }
}

}; // namespace Checkers
