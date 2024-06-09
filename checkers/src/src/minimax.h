#pragma once

#include "game.h"
#include "board.h"
#include "../include/checkers.h"

#include <map>

namespace Minimax {

  static const std::map<BoardField, int> PIECE_VALUES = {
    {WHITE_PIECE, 1},
    {WHITE_KING, 3},
    {BLACK_PIECE, -1},
    {BLACK_KING, -3}
  };

  int evaluate_board(Board &board);


  /**
   * @brief Minimax algorithm implementation with alpha-beta pruning, assumes that
   * the board is in a valid state and that the maximizing player is the white player
  */
  int minimax(Board &board, int alpha, int beta, int depth, Player player);
  
};
