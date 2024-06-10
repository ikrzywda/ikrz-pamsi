#include <stdio.h>

#include "include/checkers.h"

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
