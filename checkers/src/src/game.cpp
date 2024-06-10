#include "game.h"

#include <stdexcept>
#include <string>

namespace Checkers::Utilities {

bool is_field_black(int row, int col) {
  return (row % 2) && !(col % 2) || !(row % 2) && (col % 2);
}

void get_possible_fields(std::vector<FieldCoordinates> &possible_fields,
                         int row, int col, std::vector<DiagonalMove> &moves) {
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
} // namespace Checkers::Utilities

namespace Checkers {


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

std::vector<FieldCoordinates> &
CheckersGame::get_moves_for_piece(FieldCoordinates field_coordinates) {
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

  moves->erase(
      std::remove_if(
          moves->begin(), moves->end(),
          [this, field_coordinates](FieldCoordinates target_field_coordinates) {
            return get_move_type(field_coordinates, target_field_coordinates) ==
                   INVALID_MOVE;
          }),
      moves->end());

  return *moves;
}

MoveType CheckersGame::get_move_type(FieldCoordinates start_field,
                                     FieldCoordinates target_field) {
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

  bool is_capture_intended =
      abs(start_row - target_row) == 2 && abs(start_col - target_col) == 2;

  int start_field_index = reference_board[start_row][start_col];
  int target_field_index = reference_board[target_row][target_col];

  BoardField start_field_type = (BoardField)game_board[start_row][start_col];
  BoardField target_field_type = (BoardField)game_board[target_row][target_col];

  VerticalDirection vertical_direction = start_row - target_row > 0 ? UP : DOWN;

  if (start_field_type == EMPTY_FIELD || target_field_type != EMPTY_FIELD) {
    return INVALID_MOVE;
  }

  if (start_field_type == BLACK_PIECE && vertical_direction == UP ||
      start_field_type == WHITE_PIECE && vertical_direction == DOWN) {
    return INVALID_MOVE;
  }

  if (reference_board[start_row][start_col] == REF_OUTSIDE_FIELD ||
      reference_board[target_row][target_col] == REF_OUTSIDE_FIELD) {
    return INVALID_MOVE;
  }

  BoardField capture_field_type =
      start_field_type == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
  switch (start_field_type) {
  case BLACK_PIECE:
  case WHITE_PIECE: {
    if (is_capture_intended) {
      int jumped_field_row = start_row + vertical_direction;
      int jumped_field_col = start_col + (start_col - target_col) / 2;
      BoardField jumped_field_type =
          (BoardField)game_board[jumped_field_row][jumped_field_col];
      if (jumped_field_type == capture_field_type) {
        return start_field_type == BLACK_PIECE ? BLACK_CAPTURE : WHITE_CAPTURE;
      }
    }
    break;
  }
  case BLACK_KING:
  case WHITE_KING: {
    if (is_capture_intended) {
      int jumped_field_row = start_row + vertical_direction;
      int jumped_field_col = start_col + (start_col - target_col) / 2;
      BoardField jumped_field_type =
          (BoardField)game_board[jumped_field_row][jumped_field_col];
      if (jumped_field_type == capture_field_type) {
        return start_field_type == BLACK_KING ? BLACK_CAPTURE : WHITE_CAPTURE;
      }
    }
    break;
  }
  }
  return start_field_type == BLACK_PIECE ? BLACK_MOVE : WHITE_MOVE;
}

std::optional<std::vector<FieldCoordinates> &>
CheckersGame::get_possible_moves_for_piece(Player player,
                                           FieldCoordinates field_coordinates,
                                           bool captures_only = false) {
  const auto [row, col] = field_coordinates;
  if (!Utilities::is_field_black(row, col)) {
    return std::nullopt;
  }

  int field = game_board[row][col];
  if (field == EMPTY_FIELD) {
    return std::nullopt;
  }

  BoardField field_type = (BoardField)field;
  if (player == BLACK_PLAYER && field_type != BLACK_PIECE &&
      field_type != BLACK_KING) {
    return std::nullopt;
  }

  if (player == WHITE_PLAYER && field_type != WHITE_PIECE &&
      field_type != WHITE_KING) {
    return std::nullopt;
  }

  FieldCoordinates field_coordinates = FieldCoordinates(row, col);
  std::vector<FieldCoordinates> moves = get_moves_for_piece(field_coordinates);

  if (captures_only) {
    moves.erase(std::remove_if(moves.begin(), moves.end(),
                               [this, field_coordinates](
                                   FieldCoordinates target_field_coordinates) {
                                 return get_move_type(
                                            field_coordinates,
                                            target_field_coordinates) !=
                                        BLACK_CAPTURE;
                               }),
                moves.end());
  }

  return moves;
}

void CheckersGame::finish_game(GameStatus status) { game_status = status; }

std::optional<PossibleMoves &> CheckersGame::get_possible_moves(
    Player player, std::optional<Play> last_capture_play = std::nullopt) {
  if (last_capture_play.has_value()) {
    const auto &[last_move, last_move_type] = last_capture_play.value();
    if (player == WHITE_PLAYER && last_move_type != WHITE_CAPTURE ||
        player == BLACK_PLAYER && last_move_type != BLACK_CAPTURE) {
      return std::nullopt;
    }
    const auto &[start_field, _] = last_move;
    FieldCoordinates field_coordinates = start_field;
    auto moves_for_field =
        get_possible_moves_for_piece(player, start_field, true);
    if (moves_for_field.has_value()) {
      PossibleMoves *_possible_moves = new PossibleMoves();
      _possible_moves->emplace(start_field, moves_for_field.value());
      return *_possible_moves;
    }
    return std::nullopt;
  }

  PossibleMoves *possible_moves = new PossibleMoves();
  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {
      int _row = row + 1;
      int _col = col + 1;

      const FieldCoordinates field_coordinates = FieldCoordinates(_row, _col);
      auto moves_for_field =
          get_possible_moves_for_piece(player, field_coordinates);
      if (!moves_for_field.has_value()) {
        continue;
      }
      possible_moves->emplace(field_coordinates, moves_for_field.value());
    }
  }

  return *possible_moves;
}

void CheckersGame::make_move(FieldCoordinates start_field,
                             FieldCoordinates target_field) {
  auto [start_row, start_col] = start_field;
  auto [target_row, target_col] = target_field;

  BoardField start_field_type = (BoardField)game_board[start_row][start_col];
  Player player =
      start_field_type == BLACK_PIECE || start_field_type == BLACK_KING
          ? BLACK_PLAYER
          : WHITE_PLAYER;
  MoveType move_type = get_move_type(start_field, target_field);

  if (move_type == INVALID_MOVE) {
    throw std::invalid_argument("Invalid move");
  }

  if (move_type == BLACK_MOVE || move_type == WHITE_MOVE) {
    game_board[target_row][target_col] = start_field_type;
    game_board[start_row][start_col] = EMPTY_FIELD;
  } else if (move_type == BLACK_CAPTURE || move_type == WHITE_CAPTURE) {
    int jumped_field_row = start_row + (start_row - target_row) / 2;
    int jumped_field_col = start_col + (start_col - target_col) / 2;
    game_board[target_row][target_col] = start_field_type;
    game_board[start_row][start_col] = EMPTY_FIELD;
    game_board[jumped_field_row][jumped_field_col] = EMPTY_FIELD;
  }
}

std::optional<Play>
CheckersGame::make_minimax_move(int depth, Player player,
                                std::optional<Play> last_play) {
  BoardMove best_move;
  const auto &possible_moves = get_possible_moves(player, last_play);

  if (!possible_moves.has_value()) {
    return std::nullopt;
  }

  int best_score = player == WHITE_PLAYER ? INT_MIN : INT_MAX;

  for (auto &move : possible_moves.value()) {
    auto [starting_position, target_positions] = move;
    for (auto &target_position : target_positions) {
      CheckersGame new_game = CheckersGame(*this);
      new_game.make_move(starting_position, target_position);
      if (player == WHITE_PLAYER) {
        int score = Minimax::minimax(new_game, INT_MIN, INT_MAX, depth - 1,
                                     BLACK_PLAYER);
        if (score > best_score) {
          best_score = score;
          best_move = BoardMove(starting_position, target_position);
        }
      } else {
        int score = Minimax::minimax(new_game, INT_MIN, INT_MAX, depth - 1,
                                     WHITE_PLAYER);
        if (score < best_score) {
          best_score = score;
          best_move = BoardMove(starting_position, target_position);
        }
      }
    }
  }
  MoveType move_type = get_move_type(best_move.first, best_move.second);
  make_move(best_move.first, best_move.second);
  return Play(best_move, move_type);
}

CheckersGame::CheckersGame() {
  init_checkers_game_board();
}

CheckersGame::CheckersGame(const CheckersGame &game) {
  for (int row = 0; row < BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < BOARD_SIDE_LENGTH; ++col) {
      game_board[row][col] = game.game_board[row][col];
    }
  }
  GameStatus game_status = game.game_status;
}

std::optional<Play>
CheckersGame::play_minimax(FullPlay &play, int depth, Player player,
                           std::optional<Play> last_play = std::nullopt) {
  std::optional<Play> new_play = std::nullopt;
  if (last_play.has_value()) {
    new_play = make_minimax_move(depth, player, last_play);
  } else {
    new_play = make_minimax_move(depth, player);
  }

  if (!new_play.has_value()) {
    return std::nullopt;
  }

  play.push_back(new_play.value());
  const auto &[_, move_type] = new_play.value();
  if (move_type == BLACK_CAPTURE || move_type == WHITE_CAPTURE) {
    return play_minimax(play, depth, player, new_play);
  }

  return new_play;
}

FullPlay CheckersGame::play(int depth, Player player) {
  FullPlay *play = new FullPlay();
  play_minimax(*play, depth, player);

  if (play->empty()) {
    finish_game(player == WHITE_PLAYER ? BLACK_WON : WHITE_WON);
  }

  return *play;
}

Board const& CheckersGame::lazy_get_reference_board() { 
  if (reference_board[0][0] == REF_OUTSIDE_FIELD) {
    return reference_board;
  }

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

  return reference_board;
  }

} // namespace Checkers

