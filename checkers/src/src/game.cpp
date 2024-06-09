#include "game.h"

#include <stdexcept>

using namespace Checkers;

namespace Utilities {

bool is_field_black(int row, int col) {
  return (row % 2) && !(col % 2) || !(row % 2) && (col % 2);
}

void get_possible_fields(std::vector<FieldCoordinates> &possible_fields, int row, int col, std::vector<DiagonalMove> &moves) {
  std::vector<FieldCoordinates> possible_fields;

  for (auto move : moves) {
    auto [row_move, col_move] = DIAGONAL_MOVES[move];

    for (int i = 1; i <= 2; ++i) {
      int _row = row + row_move * i;
      int _col = col + col_move * i;

      possible_fields.push_back(FieldCoordinates(_row, _col));
    }
  }
}

void CheckersGame::init_checkers_reference_board() {
  for (int i = 0; i < BOARD_SIDE_LENGTH; ++i) {
    reference_board[0][i] = REF_OUTSIDE_FIELD;
    reference_board[BOARD_SIDE_LENGTH - 1][i] = REF_OUTSIDE_FIELD;
    reference_board[i][0] = REF_OUTSIDE_FIELD;
    reference_board[i][BOARD_SIDE_LENGTH - 1] = REF_OUTSIDE_FIELD;
  }

  int field_index = 0;
  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {
      int _row = row + 1;
      int _col = col + 1;

      if (Utilities::is_field_black(row, col)) {
        reference_board[_row][_col] = ++field_index;
      } else {
        reference_board[_row][_col] = REF_WHITE_FIELD;
      }
    }
  }
}

void CheckersGame::init_checkers_game_board() {
  for (int i = 0; i < BOARD_SIDE_LENGTH; ++i) {
    game_board[0][i] = OUTSIDE_FIELD;
    game_board[BOARD_SIDE_LENGTH - 1][i] = OUTSIDE_FIELD;
    game_board[i][0] = OUTSIDE_FIELD;
    game_board[i][BOARD_SIDE_LENGTH - 1] = OUTSIDE_FIELD;
  }
  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {
      if (!Utilities::is_field_black(row, col)) {
        int _row = row + 1;
        int _col = col + 1;

        if (Utilities::is_field_black(row, col) && row < 3) {
          game_board[_row][_col] = BLACK_PIECE;
        } else if (Utilities::is_field_black(row, col) && row > 4) {
          game_board[_row][_col] = WHITE_PIECE;
        } else {
          game_board[_row][_col] = EMPTY_FIELD;
        }
      }
    }
  }
}

FieldCoordinates CheckersGame::get_field_coordinates(int field_index) {
  if (field_index > REF_MAX_FIELD_INDEX || field_index < 0) {
    throw std::invalid_argument("Field index out of bounds");
  }

  int row = field_index / ACTIVE_BOARD_SIDE_LENGTH;
  int col = field_index % ACTIVE_BOARD_SIDE_LENGTH;

  return std::make_tuple(row, col);
}

BoardField CheckersGame::get_game_field(int field_index) {
  if (field_index > REF_MAX_FIELD_INDEX || field_index < 0) {
    throw std::invalid_argument("Field index out of bounds");
  }

  int row = field_index / ACTIVE_BOARD_SIDE_LENGTH;
  int col = field_index % ACTIVE_BOARD_SIDE_LENGTH;
  int field = reference_board[row][col];

  return (BoardField)field;
}

std::vector<FieldCoordinates> &CheckersGame::get_moves_for_piece(FieldCoordinates field_coordinates) {
  std::vector<FieldCoordinates> *moves = new std::vector<FieldCoordinates>(); 
  std::vector<DiagonalMove> diagonal_moves;
  
  auto [row, col] = field_coordinates;
  int field = game_board[row][col];

  switch (field) {
    case BLACK_PIECE: {
      diagonal_moves = {TOP_LEFT, TOP_RIGHT};
      break;
    }
    case WHITE_PIECE: {
      diagonal_moves = {BOTTOM_LEFT, BOTTOM_RIGHT};
      break;
    }
    case BLACK_KING:
    case WHITE_KING: {
      diagonal_moves = {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};
      break;
    }
  }

  Utilities::get_possible_fields(*moves, row, col, diagonal_moves);
  return *moves;
}

MoveType CheckersGame::get_move_type(FieldCoordinates start_field, FieldCoordinates target_field) {
  if (start_field == target_field) {
    return INVALID_MOVE;
  }

  auto [start_row, start_col] = start_field;
  auto [target_row, target_col] = target_field;

  if (abs(start_row - target_row) != abs(start_col - target_col)) {
    return INVALID_MOVE;
  }

  if (abs(start_row - target_row) > 2 || abs(start_col - target_col) > 2) {
    return INVALID_MOVE;
  }

  bool is_capture_intended = abs(start_row - target_row) == 2 && abs(start_col - target_col) == 2;

  int start_field_index = reference_board[start_row][start_col];
  int target_field_index = reference_board[target_row][target_col];

  BoardField start_field_type = (BoardField)game_board[start_row][start_col];
  BoardField target_field_type = (BoardField)game_board[target_row][target_col];
  
  VerticalDirection vertical_direction = start_row - target_row > 0 ? UP : DOWN;
  
  if (start_field_type == EMPTY_FIELD || target_field_type != EMPTY_FIELD) {
    return INVALID_MOVE;
  }

  if (start_field_type == BLACK_PIECE && vertical_direction == UP || start_field_type == WHITE_PIECE && vertical_direction == DOWN) {
    return INVALID_MOVE;
  }

  BoardField capture_field_type = start_field_type == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
  switch (start_field_type) {
    case BLACK_PIECE:
    case WHITE_PIECE: {
      if (is_capture_intended) {
        int jumped_field_row = start_row + vertical_direction;
        int jumped_field_col = start_col + (start_col - target_col) / 2;
        BoardField jumped_field_type = (BoardField)game_board[jumped_field_row][jumped_field_col];
        if (jumped_field_type == capture_field_type) {
          return start_field_type == BLACK_PIECE ? BLACK_CAPTURE : WHITE_CAPTURE;
        }
      }
    }
    case BLACK_KING:
    case WHITE_KING: {
      if (is_capture_intended) {
        int jumped_field_row = start_row + vertical_direction;
        int jumped_field_col = start_col + (start_col - target_col) / 2;
        BoardField jumped_field_type = (BoardField)game_board[jumped_field_row][jumped_field_col];
        if (jumped_field_type == capture_field_type) {
          return start_field_type == BLACK_KING ? BLACK_CAPTURE : WHITE_CAPTURE;
        }
      }
    }
  }
  return start_field_type == BLACK_PIECE ? BLACK_MOVE : WHITE_MOVE;
}

PossibleMoves &CheckersGame::get_possible_moves(Player player) {
  PossibleMoves *possible_moves = new PossibleMoves();

  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {

      int _row = row + 1;
      int _col = col + 1;

      if (!Utilities::is_field_black(_row, _col)) {
        continue;
      }

      int field = game_board[_row][_col];
      if (field == EMPTY_FIELD) {
        continue;
      }

      BoardField field_type = (BoardField)field;
      if (player == BLACK_PLAYER && field_type != BLACK_PIECE && field_type != BLACK_KING) {
        continue;
      }

      if (player == WHITE_PLAYER && field_type != WHITE_PIECE && field_type != WHITE_KING) {
        continue;
      }

      FieldCoordinates field_coordinates = FieldCoordinates(_row, _col);
      std::vector<FieldCoordinates> moves = get_moves_for_piece(field_coordinates);
      possible_moves->emplace(field_coordinates, moves);
    }
  }

  return *possible_moves;
}


CheckersGame::CheckersGame() {
  init_checkers_reference_board();
  init_checkers_game_board();
}
