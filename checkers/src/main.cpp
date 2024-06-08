#include <stdio.h>

#define REFERENCE_BOARD_EMPTY_FIELD -1
#define BOARD_SIDE_LENGTH 10
#define ACTIVE_BOARD_SIDE_LENGTH 8
#define BOARD_SIZE BOARD_SIDE_LENGTH * BOARD_SIDE_LENGTH
#define ACTIVE_BOARD_SIZE ACTIVE_BOARD_SIDE_LENGTH * ACTIVE_BOARD_SIDE_LENGTH


enum ReferenceBoardField {
  REF_OUTSIDE_FIELD = -2,
  REF_WHITE_FIELD = -1
};

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

bool is_black_field(int row, int col) {
  return (row % 2) && !(col % 2) || !(row % 2) && (col % 2);
}



struct CheckersGame {
  Board reference_board;
  void init_checkers_reference_board();
  void init_checkers_game_board();
  void print_state();

  Board game_board;
  CheckersGame();
};

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

      if (is_black_field(row, col)) {
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
      if (!is_black_field(row, col)) {
        int _row = row + 1;
        int _col = col + 1;

        if (is_black_field(row, col) && row < 3) {
          game_board[_row][_col] = BLACK_PIECE;
        } else if (is_black_field(row, col) && row > 4) {
          game_board[_row][_col] = WHITE_PIECE;
        } else {
          game_board[_row][_col] = EMPTY_FIELD;
        }
      }
    }
  }
}

CheckersGame::CheckersGame() {
  init_checkers_reference_board();
  init_checkers_game_board();
}

void print_checkers_board(Board board) {
  for (int row = 0; row < BOARD_SIDE_LENGTH; ++row) {
    for (int col = 0; col < BOARD_SIDE_LENGTH; ++col) {
      int _row = row + 1;
      if (board[row][col] == EMPTY_FIELD) {
        printf("   ");
      } else {
        printf("%3d", board[row][col]);
      }
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {

  return 0;
}
