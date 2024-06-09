#include "minimax.h"

using namespace Minimax;

int evaluate_board(Board &board) {
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


int minimax(Checkers::CheckersGame &game, int alpha, int beta, int depth, Player player) {
  if (!depth) {
    return Minimax::evaluate_board(game.game_board);
  }

  Checkers::PossibleMoves &possible_moves = game.get_possible_moves(player);
  if (possible_moves.empty()) {
    return Minimax::evaluate_board(game.game_board);
  }

  switch (player) {
    case WHITE_PLAYER: {
      int max_eval = INT_MIN;
      for (auto &move : possible_moves) {
        auto [starting_position, moves] = move;

      }
    }
  }

  return 0;
}
