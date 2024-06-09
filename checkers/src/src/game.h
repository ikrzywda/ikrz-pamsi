#pragma once

#include <limits.h>

#include <map>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "board.h"

namespace Checkers {

enum VerticalDirection { UP = -1, DOWN = 1 };

enum DiagonalMove {
  TOP_LEFT = 0,
  TOP_RIGHT = 1,
  BOTTOM_LEFT = 2,
  BOTTOM_RIGHT = 3
};

enum GameStatus { GAME_IN_PROGRESS, BLACK_WON, WHITE_WON, DRAW };

enum MoveType {
  WHITE_MOVE,
  BLACK_MOVE,
  WHITE_CAPTURE,
  BLACK_CAPTURE,
  INVALID_MOVE,
};


using FieldCoordinates = std::pair<int, int>;
struct BoardMove {
  MoveType move_type;
  FieldCoordinates start_field;
  std::vector<FieldCoordinates> target_fields;

  std::string encode();
  static BoardMove decode(std::string move);
};

using PossibleMoves =
    std::map<FieldCoordinates, std::vector<FieldCoordinates> &>;


static const FieldCoordinates DIAGONAL_MOVES[] = {
    FieldCoordinates(-1, -1), FieldCoordinates(-1, 1), FieldCoordinates(1, -1),
    FieldCoordinates(1, 1)};

namespace Utilities {
bool is_field_black(int row, int col);
void get_possible_fields(std::vector<FieldCoordinates> &possible_fields,
                         int row, int col, std::vector<DiagonalMove> &moves);
} // namespace Utilities

struct CheckersGame {
  Board game_board;
  GameStatus game_status;

  CheckersGame();
  CheckersGame(const CheckersGame &game);
  PossibleMoves &get_possible_moves(Player player);
  void make_move(FieldCoordinates start_field, FieldCoordinates target_field);
  void make_minimax_move(int depth, Player player);


private:
  Board reference_board;
  void init_checkers_reference_board();
  void init_checkers_game_board();

  std::vector<FieldCoordinates> &
  get_moves_for_piece(FieldCoordinates field_coordinates);

  BoardField get_game_field(int field_index);
  FieldCoordinates get_field_coordinates(int field_index);

  MoveType get_move_type(FieldCoordinates start_field,
                        FieldCoordinates target_field);
};

namespace Minimax {
static const std::map<BoardField, int> PIECE_VALUES = {
    {WHITE_PIECE, 1}, {WHITE_KING, 3}, {BLACK_PIECE, -1}, {BLACK_KING, -3}};

int evaluate_board(Board &board);

/**
 * @brief Minimax algorithm implementation with alpha-beta pruning, assumes that
 * the board is in a valid state and that the maximizing player is the white
 * player
 */
int minimax(CheckersGame &game, int alpha, int beta, int depth,
            Player player);
} // namespace Minimax


namespace NotationEncoder {
std::string encode_move(BoardMove move, MoveType move_type);
BoardMove decode_move(std::string move);
}


} // namespace Checkers
