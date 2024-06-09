#pragma once

#include "board.h"

#include <tuple>
#include <stdexcept>
#include <vector>
#include <map>
#include <optional>
#include <limits.h>

namespace Checkers {

using FieldCoordinates = std::tuple<int, int>;
using PossibleMoves = std::map<FieldCoordinates, std::vector<FieldCoordinates>&>;

enum VerticalDirection {
  UP = -1,
  DOWN = 1
};

enum DiagonalMove {
  TOP_LEFT = 0,
  TOP_RIGHT = 1,
  BOTTOM_LEFT = 2,
  BOTTOM_RIGHT = 3
};

enum GameStatus {
  GAME_IN_PROGRESS,
  BLACK_WON,
  WHITE_WON,
  DRAW
};

enum MoveType {
  WHITE_MOVE,
  BLACK_MOVE,
  WHITE_CAPTURE,
  BLACK_CAPTURE,
  INVALID_MOVE,
};


static const FieldCoordinates DIAGONAL_MOVES[] = {
  FieldCoordinates(-1, -1),
  FieldCoordinates(-1, 1),
  FieldCoordinates(1, -1),
  FieldCoordinates(1, 1)
};

namespace Utilities {
  bool is_field_black(int row, int col);
  void get_possible_fields(std::vector<FieldCoordinates> &possible_fields, int row, int col, std::vector<DiagonalMove> &moves);
}


struct CheckersGame {
  Board game_board;
  GameStatus game_status;

  CheckersGame();
  PossibleMoves &get_possible_moves(Player player);
  void make_move(FieldCoordinates start_field, FieldCoordinates target_field, std::optional<MoveType> last_move);

  private:
    Board reference_board;
    void init_checkers_reference_board();
    void init_checkers_game_board();

    std::vector<FieldCoordinates> &get_moves_for_piece(FieldCoordinates field_coordinates);

    MoveType get_move_type(FieldCoordinates start_field, FieldCoordinates target_field);
    BoardField get_game_field(int field_index);
    FieldCoordinates get_field_coordinates(int field_index);
};


} // namespace Checkers
