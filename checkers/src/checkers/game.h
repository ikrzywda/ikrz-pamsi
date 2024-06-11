#pragma once

#include <limits.h>

#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "board.h"

namespace Checkers {

#define REFERENCE_BOARD_EMPTY_FIELD -1
#define BOARD_SIDE_LENGTH 10
#define ACTIVE_BOARD_SIDE_LENGTH 8
#define BOARD_SIZE BOARD_SIDE_LENGTH *BOARD_SIDE_LENGTH
#define ACTIVE_BOARD_SIZE ACTIVE_BOARD_SIDE_LENGTH *ACTIVE_BOARD_SIDE_LENGTH
#define REF_MAX_FIELD_INDEX 32

enum class Player { BLACK = 0, WHITE = 1 };

enum ReferenceBoardField { REF_OUTSIDE_FIELD = -2, REF_WHITE_FIELD = -1 };

enum BoardField {
  BLACK_PIECE = 51,
  WHITE_PIECE = 52,
  BLACK_KING = 53,
  WHITE_KING = 54,
  EMPTY_FIELD = 55,
  INVALID_FIELD = 56,
  OUTSIDE_FIELD = 57
};

typedef int Board[BOARD_SIDE_LENGTH][BOARD_SIDE_LENGTH];

enum VerticalDirection { UP = -1, DOWN = 1 };
enum HorizontalDirection { LEFT = -1, RIGHT = 1 };

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

using PossibleMovesForPiece = std::pair<std::vector<FieldCoordinates>, bool>;
using MovesMap = std::map<FieldCoordinates, PossibleMovesForPiece>;
using PossibleMoves = std::pair<MovesMap, bool>;

using BoardMove = std::pair<FieldCoordinates, FieldCoordinates>;
using Play = std::pair<BoardMove, MoveType>;
using Turn = std::vector<Play>;

static const FieldCoordinates DIAGONAL_MOVES[] = {
    FieldCoordinates(-1, -1), FieldCoordinates(-1, 1), FieldCoordinates(1, -1),
    FieldCoordinates(1, 1)};

struct CheckersGame {
  static Board const &lazy_get_reference_board();
  static std::optional<FieldCoordinates> translate_field_index(int field_index);

  Board game_board;
  GameStatus game_status;

  CheckersGame();

  std::optional<PossibleMoves>
  get_possible_moves(Player player,
                     std::optional<Play> last_capture_play = std::nullopt);

  void make_move(FieldCoordinates start_field, FieldCoordinates target_field);
  std::optional<Turn> play_ai_turn(int depth, Player player);
  void print_board();

private:
  static Board reference_board;
  void init_checkers_game_board();

  std::vector<FieldCoordinates>
  get_moves_for_piece(FieldCoordinates field_coordinates);

  std::optional<PossibleMovesForPiece> get_possible_moves_for_piece(
      Player player, FieldCoordinates field_coordinates, bool captures_only);
  FieldCoordinates get_field_coordinates(int field_index);

  MoveType get_move_type(FieldCoordinates start_field,
                         FieldCoordinates target_field);

  std::optional<Play> play_minimax(Turn &play, int depth, Player player,
                                   std::optional<Play> last_play);
  std::optional<Play> make_minimax_move(int depth, Player player,
                                        std::optional<Play> last_play);
  void finish_game(GameStatus status);
};

} // namespace Checkers

namespace Checkers::Utilities {
bool is_field_black(int row, int col);
std::vector<FieldCoordinates>
get_possible_fields(int row, int col, std::vector<DiagonalMove> &moves);
std::optional<int> get_field_index(int row, int col);
} // namespace Checkers::Utilities

namespace Checkers::Minimax {
static const std::map<BoardField, int> PIECE_VALUES = {
    {WHITE_PIECE, 1}, {WHITE_KING, 3}, {BLACK_PIECE, -1}, {BLACK_KING, -3}};

int evaluate_board(Board &board);
int minimax(CheckersGame &game, int alpha, int beta, int depth, Player player);
} // namespace Checkers::Minimax

namespace Checkers::NotationTranslation {

#define CAPTURE_SYMBOL 'x'
#define MOVE_SYMBOL '-'

using EncodedMove = std::string;

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

template <typename T>
using ParseResult = std::pair<std::optional<T>, std::optional<ParseError>>;

using Token = std::pair<TokenId, std::string>;
EncodedMove encode_play(Turn const &play);
std::vector<Token> tokenize_move(std::string const &move);

struct MoveDecoder {
  static ParseResult<Turn> decode_move(std::string const &move);

private:
  std::string move;
  std::vector<Token> tokens;
  int current_token_index;

  MoveDecoder(std::string const &move);
  Token get_next_token();
  Token peek_next_token();
  Token get_current_token();
  bool has_next_token();

  ParseResult<FieldCoordinates> decode_position();
  ParseResult<Turn> decode_play();
  ParseResult<MoveType> decode_move_type();
};

} // namespace Checkers::NotationTranslation
