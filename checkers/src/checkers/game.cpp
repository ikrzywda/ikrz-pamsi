#include "game.h"

#include <stdexcept>
#include <string>

namespace Checkers::Utilities {

bool is_field_black(int row, int col) {
  if (row % 2) {
    return col % 2 == 0;
  } else {
    return col % 2 == 1;
  }
}

std::vector<FieldCoordinates>
get_possible_fields(int row, int col, std::vector<DiagonalMove> &moves) {
  std::vector<FieldCoordinates> possible_fields =
      std::vector<FieldCoordinates>();
  for (auto move : moves) {
    auto [row_move, col_move] = DIAGONAL_MOVES[move];
    for (int i = 1; i <= 2; ++i) {
      int _row = row + row_move * i;
      int _col = col + col_move * i;

      possible_fields.push_back(FieldCoordinates(_row, _col));
    }
  }
  return possible_fields;
}

std::optional<int> get_field_index(int row, int col) {
  auto reference_board = CheckersGame::lazy_get_reference_board();

  std::cout << "Row: " << row << " Col: " << col << std::endl;

  if (row < 0 || row >= BOARD_SIDE_LENGTH || col < 0 ||
      col >= BOARD_SIDE_LENGTH) {
    return std::nullopt;
  }

  return reference_board[row][col];
}

} // namespace Checkers::Utilities

namespace Checkers {

Board CheckersGame::reference_board;

void CheckersGame::init_checkers_game_board() {
  for (int i = 0; i < BOARD_SIDE_LENGTH; ++i) {
    game_board[0][i] = OUTSIDE_FIELD;
    game_board[BOARD_SIDE_LENGTH - 1][i] = OUTSIDE_FIELD;
    game_board[i][0] = OUTSIDE_FIELD;
    game_board[i][BOARD_SIDE_LENGTH - 1] = OUTSIDE_FIELD;
  }
  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {
      int _row = row + 1;
      int _col = col + 1;
      if (Utilities::is_field_black(_row, _col)) {
        if (_row < 4) {
          game_board[_row][_col] = BLACK_PIECE;
        } else if (_row > 5) {
          game_board[_row][_col] = WHITE_PIECE;
        } else {
          game_board[_row][_col] = EMPTY_FIELD;
        }
      } else {
        game_board[_row][_col] = EMPTY_FIELD;
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

std::vector<FieldCoordinates>
CheckersGame::get_moves_for_piece(FieldCoordinates field_coordinates) {
  std::vector<DiagonalMove> diagonal_moves;

  auto [row, col] = field_coordinates;
  int field = game_board[row][col];

  switch (field) {
  case BLACK_PIECE: {
    diagonal_moves = {BOTTOM_LEFT, BOTTOM_RIGHT};
    break;
  }
  case WHITE_PIECE: {
    diagonal_moves = {TOP_LEFT, TOP_RIGHT};
    break;
  }
  case BLACK_KING:
  case WHITE_KING: {
    diagonal_moves = {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};
    break;
  }
  }

  auto possible_moves =
      Utilities::get_possible_fields(row, col, diagonal_moves);

  possible_moves.erase(
      std::remove_if(
          possible_moves.begin(), possible_moves.end(),
          [this, field_coordinates](FieldCoordinates target_field_coordinates) {
            return get_move_type(field_coordinates, target_field_coordinates) ==
                   INVALID_MOVE;
          }),
      possible_moves.end());

  return possible_moves;
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

  BoardField start_field_type = (BoardField)game_board[start_row][start_col];
  BoardField target_field_type = (BoardField)game_board[target_row][target_col];

  VerticalDirection vertical_direction = start_row - target_row > 0 ? UP : DOWN;
  HorizontalDirection horizontal_direction =
      start_col - target_col > 0 ? LEFT : RIGHT;

  if (start_field_type == EMPTY_FIELD || target_field_type != EMPTY_FIELD) {
    return INVALID_MOVE;
  }

  if ((start_field_type == BLACK_PIECE && vertical_direction == UP) ||
      (start_field_type == WHITE_PIECE && vertical_direction == DOWN)) {
    return INVALID_MOVE;
  }

  BoardField capture_field_type =
      start_field_type == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
  switch (start_field_type) {
  case BLACK_PIECE:
  case WHITE_PIECE: {
    if (is_capture_intended) {
      int jumped_field_row = start_row + vertical_direction;
      int jumped_field_col = start_col + horizontal_direction;
      BoardField jumped_field_type =
          (BoardField)game_board[jumped_field_row][jumped_field_col];

      if (jumped_field_type == capture_field_type) {
        return start_field_type == BLACK_PIECE ? BLACK_CAPTURE : WHITE_CAPTURE;
      } else {
        return INVALID_MOVE;
      }
    }
    break;
  }
  case BLACK_KING:
  case WHITE_KING: {
    if (is_capture_intended) {
      int jumped_field_row = start_row + vertical_direction;
      int jumped_field_col = start_col + horizontal_direction;
      BoardField jumped_field_type =
          (BoardField)game_board[jumped_field_row][jumped_field_col];
      if (jumped_field_type == capture_field_type) {
        return start_field_type == BLACK_KING ? BLACK_CAPTURE : WHITE_CAPTURE;
      } else {
        return INVALID_MOVE;
      }
    }
    break;
  }
  default: {
    return INVALID_MOVE;
  }
  }
  return start_field_type == BLACK_PIECE ? BLACK_MOVE : WHITE_MOVE;
}

std::optional<PossibleMovesForPiece> CheckersGame::get_possible_moves_for_piece(
    Player player, FieldCoordinates field_coordinates, bool captures_only) {
  const auto [row, col] = field_coordinates;
  int field = game_board[row][col];

  if (field == EMPTY_FIELD) {
    return std::nullopt;
  }

  if (player == Player::BLACK && field != BLACK_PIECE && field != BLACK_KING) {
    return std::nullopt;
  }

  if (player == Player::WHITE && field != WHITE_PIECE && field != WHITE_KING) {
    return std::nullopt;
  }

  std::vector<FieldCoordinates> moves = get_moves_for_piece(field_coordinates);

  bool has_captures = std::accumulate(
      moves.begin(), moves.end(), false,
      [this, field_coordinates](bool acc, FieldCoordinates current) {
        MoveType move_type = get_move_type(field_coordinates, current);
        return acc || move_type == MoveType::BLACK_CAPTURE ||
               move_type == MoveType::WHITE_CAPTURE;
      });

  std::cout << "HAS CAPTURES: " << has_captures << std::endl;

  bool _captures_only = captures_only || has_captures;

  std::cout << "CAPTURES ONLY: " << _captures_only << std::endl;
  if (_captures_only) {
    moves.erase(std::remove_if(moves.begin(), moves.end(),
                               [this, field_coordinates](
                                   FieldCoordinates target_field_coordinates) {
                                 auto move_type =
                                     get_move_type(field_coordinates,
                                                   target_field_coordinates);
                                 return move_type != MoveType::BLACK_CAPTURE &&
                                        move_type != MoveType::WHITE_CAPTURE;
                               }),
                moves.end());
  }

  return PossibleMovesForPiece(moves, has_captures);
}

void CheckersGame::finish_game(GameStatus status) { game_status = status; }

std::optional<PossibleMoves>
CheckersGame::get_possible_moves(Player player,
                                 std::optional<Play> last_capture_play) {
  if (last_capture_play.has_value()) {
    std::cout << "LAST CAPTURE PLAY_--___" << std::endl;
    const auto &[last_move, last_move_type] = last_capture_play.value();
    // if ((player == Player::WHITE && last_move_type != WHITE_CAPTURE) ||
    //     (player == Player::BLACK && last_move_type != BLACK_CAPTURE)) {
    //   return std::nullopt;
    // }
    const auto &[start_field, target_field] = last_move;
    std::cout << "Start field: " << start_field.first << " "
              << start_field.second << std::endl;
    auto moves_for_field =
        get_possible_moves_for_piece(player, target_field, true);
    if (!moves_for_field.has_value()) {
      std::cout << "NO MOVES FOR FIELD" << std::endl;
      return std::nullopt;
    }

    MovesMap _possible_moves = MovesMap();
    auto [_, has_captures] = moves_for_field.value();
    if (!has_captures) {
      std::cout << "NO CAPTURES" << std::endl;
      return std::nullopt;
    }
    _possible_moves.emplace(start_field, moves_for_field.value());
    return PossibleMoves(_possible_moves, has_captures);
  }

  MovesMap possible_moves = MovesMap();
  bool has_captures = false;
  for (int row = 0; row < ACTIVE_BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < ACTIVE_BOARD_SIDE_LENGTH; ++col) {
      int _row = row + 1;
      int _col = col + 1;

      const FieldCoordinates field_coordinates = FieldCoordinates(_row, _col);
      auto _possible_moves =
          get_possible_moves_for_piece(player, field_coordinates, false);
      if (!_possible_moves.has_value()) {
        continue;
      }
      auto [_, _has_captures] =
          get_possible_moves_for_piece(player, field_coordinates, false)
              .value();

      if (has_captures && !_has_captures) {
        continue;
      }
      has_captures = has_captures || _has_captures;
      possible_moves.emplace(field_coordinates, _possible_moves.value());
    }
  }

  return PossibleMoves(possible_moves, has_captures);
}

void CheckersGame::make_move(FieldCoordinates start_field,
                             FieldCoordinates target_field) {

  auto [start_row, start_col] = start_field;
  auto [target_row, target_col] = target_field;

  BoardField start_field_type = (BoardField)game_board[start_row][start_col];
  MoveType move_type = get_move_type(start_field, target_field);

  VerticalDirection vertical_direction = start_row - target_row > 0 ? UP : DOWN;
  HorizontalDirection horizontal_direction =
      start_col - target_col > 0 ? LEFT : RIGHT;

  if (move_type == INVALID_MOVE) {
    throw std::invalid_argument("make_move: Invalid move type");
  }

  if (move_type == BLACK_MOVE || move_type == WHITE_MOVE) {
    game_board[target_row][target_col] = start_field_type;
    game_board[start_row][start_col] = EMPTY_FIELD;
  } else if (move_type == BLACK_CAPTURE || move_type == WHITE_CAPTURE) {
    int jumped_field_row = start_row + vertical_direction;
    int jumped_field_col = start_col + horizontal_direction;
    game_board[target_row][target_col] = start_field_type;
    game_board[start_row][start_col] = EMPTY_FIELD;
    game_board[jumped_field_row][jumped_field_col] = EMPTY_FIELD;
  }

  if (start_field_type == BoardField::BLACK_PIECE &&
      target_row == BOARD_SIDE_LENGTH - 2) {
    game_board[target_row][target_col] = BoardField::BLACK_KING;
  } else if (start_field_type == BoardField::BLACK_PIECE && target_row == 1) {
    game_board[target_row][target_col] = BoardField::WHITE_KING;
  }
}

std::optional<Play>
CheckersGame::make_minimax_move(int depth, Player player,
                                std::optional<Play> last_play) {
  BoardMove best_move;
  auto _possible_moves = get_possible_moves(player, last_play);

  if (!_possible_moves.has_value()) {
    std::cout << "NO POSSIBLE MOVES____" << std::endl;
    return std::nullopt;
  }

  auto [possible_moves, has_captures] = _possible_moves.value();

  int best_score = player == Player::WHITE ? INT_MIN : INT_MAX;

  for (auto &move : possible_moves) {
    auto [starting_position, possible_moves] = move;
    auto [target_positions, _has_captures] = possible_moves;

    if (has_captures && !_has_captures) {
      continue;
    }

    for (auto &target_position : target_positions) {
      CheckersGame new_game = CheckersGame(*this);
      new_game.make_move(starting_position, target_position);
      if (player == Player::WHITE) {
        int score = Minimax::minimax(new_game, INT_MIN, INT_MAX, depth - 1,
                                     Player::BLACK);

        if (score > best_score) {
          best_score = score;
          best_move = BoardMove(starting_position, target_position);
        }
      } else {
        int score = Minimax::minimax(new_game, INT_MIN, INT_MAX, depth - 1,
                                     Player::WHITE);
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
  lazy_get_reference_board();
  init_checkers_game_board();
}

std::optional<Play> CheckersGame::play_minimax(Turn &play, int depth,
                                               Player player,
                                               std::optional<Play> last_play) {

  std::optional<Play> new_play = std::nullopt;
  if (last_play.has_value()) {
    std::cout << "LAST PLAY" << std::endl;
    new_play = make_minimax_move(depth, player, last_play);
  } else {
    std::cout << "CHUJNIA" << std::endl;
    new_play = make_minimax_move(depth, player, std::nullopt);
  }

  if (!new_play.has_value()) {
    std::cout << "NO NEW PLAY" << std::endl;
    return std::nullopt;
  }

  play.push_back(new_play.value());
  const auto &[_, move_type] = new_play.value();
  if (move_type == BLACK_CAPTURE || move_type == WHITE_CAPTURE) {
    std::cout << "RECURSING" << std::endl;
    return play_minimax(play, depth, player, new_play);
  }

  std::cout << "MINMAX LENGTH: " << play.size() << std::endl;
  return new_play;
}

std::optional<Turn> CheckersGame::play_ai_turn(int depth, Player player) {
  Turn turn = Turn();
  play_minimax(turn, depth, player, std::nullopt);

  if (turn.empty()) {
    finish_game(player == Player::WHITE ? BLACK_WON : WHITE_WON);
    return std::nullopt;
  }

  return turn;
}

Board const &CheckersGame::lazy_get_reference_board() {
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

std::optional<FieldCoordinates>
CheckersGame::translate_field_index(int field_index) {
  if (field_index > REF_MAX_FIELD_INDEX || field_index < 0) {
    return std::nullopt;
  }

  Board const &reference_board = CheckersGame::lazy_get_reference_board();

  for (int row = 0; row < BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < BOARD_SIDE_LENGTH; ++col) {
      if (reference_board[row][col] == field_index) {
        return FieldCoordinates(row, col);
      }
    }
  }
  return std::nullopt;
}

void CheckersGame::print_board() {
  for (int row = 0; row < BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < BOARD_SIDE_LENGTH; ++col) {
      std::cout << game_board[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
} // namespace Checkers
