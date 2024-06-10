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

using PossibleMoves =
    std::map<FieldCoordinates, std::vector<FieldCoordinates> &>;

using BoardMove = std::pair<FieldCoordinates, FieldCoordinates>;
using Play = std::pair<BoardMove, MoveType>;
using FullPlay = std::vector<Play>;

static const FieldCoordinates DIAGONAL_MOVES[] = {
    FieldCoordinates(-1, -1), FieldCoordinates(-1, 1), FieldCoordinates(1, -1),
    FieldCoordinates(1, 1)};

struct CheckersGame {
  static Board const& lazy_get_reference_board();

  Board game_board;
  GameStatus game_status;

  CheckersGame();
  CheckersGame(const CheckersGame &game);
  std::optional<PossibleMoves &>
  get_possible_moves(Player player,
                     std::optional<Play> last_capture_play = std::nullopt);
  void make_move(FieldCoordinates start_field, FieldCoordinates target_field);  
  FullPlay play(int depth, Player player);

private:
  static Board reference_board;
  void init_checkers_game_board();

  std::vector<FieldCoordinates> &
  get_moves_for_piece(FieldCoordinates field_coordinates);

  std::optional<std::vector<FieldCoordinates> &>
  get_possible_moves_for_piece(Player player,
                               FieldCoordinates field_coordinates,
                               bool captures_only = false);
  BoardField get_game_field(int field_index);
  FieldCoordinates get_field_coordinates(int field_index);

  MoveType get_move_type(FieldCoordinates start_field,
                         FieldCoordinates target_field);

  std::optional<Play>
  play_minimax(FullPlay &play, int depth, Player player,
               std::optional<Play> last_play = std::nullopt);
  std::optional<Play>
  make_minimax_move(int depth, Player player,
                    std::optional<Play> last_play = std::nullopt);
  void finish_game(GameStatus status);
};

} // namespace Checkers


namespace Checkers::Utilities {
bool is_field_black(int row, int col);
void get_possible_fields(std::vector<FieldCoordinates> &possible_fields,
                         int row, int col, std::vector<DiagonalMove> &moves);
} // namespace Checkers::Utilities

namespace Checkers::Minimax {
static const std::map<BoardField, int> PIECE_VALUES = {
    {WHITE_PIECE, 1}, {WHITE_KING, 3}, {BLACK_PIECE, -1}, {BLACK_KING, -3}};

int evaluate_board(Board &board);
int minimax(CheckersGame &game, int alpha, int beta, int depth, Player player);
} // namespace Checkers::Minimax

namespace Checkers::NotationEncoder {

#define CAPTURE_SYMBOL 'x'
#define MOVE_SYMBOL '-'

enum TokenId {
  START,
  END,
  POSITION_INDEX,
  CAPTURE,
  MOVE,
};

enum ParseError {
  INVALID_TOKEN,
  INVALID_POSITION,
  TOO_MANY_MOVES,
  INVALID_MOVE
};

using Token = std::pair<TokenId, std::string>;
using DecodingResult = std::tuple<std::optional<FullPlay&>, ParseError>;

std::string encode_play(const FullPlay const& play);
std::string encode_move(const BoardMove &move);

std::vector<Token> tokenize_move(std::string const& move);
FullPlay decode_move(std::string const& move);
} // namespace Checkers::NotationEncoder
