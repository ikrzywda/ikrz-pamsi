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

  Checkers::PossibleMoves &possible_moves = game.get_possible_moves(player);
  if (possible_moves.empty()) {
    return evaluate_board(game.game_board);
  }

  switch (player) {
  case WHITE_PLAYER: {
    int max_eval = INT_MIN;
    for (auto &move : possible_moves) {
      auto [starting_position, target_positions] = move;
      for (auto &target_position : target_positions) {
        Checkers::CheckersGame new_game = Checkers::CheckersGame(game);
        new_game.make_move(starting_position, target_position);
        int eval_score =
            minimax(new_game, alpha, beta, depth - 1, BLACK_PLAYER);
        max_eval = std::max(max_eval, eval_score);
        alpha = std::max(alpha, eval_score);
        if (beta <= alpha) {
          break;
        }
      }
    }
    return max_eval;
  }
  case BLACK_PLAYER: {
    int min_eval = INT_MAX;
    for (auto &move : possible_moves) {
      auto [starting_position, target_positions] = move;
      for (auto &target_position : target_positions) {
        Checkers::CheckersGame new_game = Checkers::CheckersGame(game);
        new_game.make_move(starting_position, target_position);
        int eval_score =
            minimax(new_game, alpha, beta, depth - 1, WHITE_PLAYER);
        min_eval = std::min(min_eval, eval_score);
        beta = std::min(beta, eval_score);
        if (beta <= alpha) {
          break;
        }
      }
    }
    return min_eval;
  }
  }
}

}; // namespace Checkers
