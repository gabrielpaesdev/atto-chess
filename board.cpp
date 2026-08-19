#include "board.h"

int8_t board[BOARD_SIZE][BOARD_SIZE] = {
    {-4, -2, -3, -5, -6, -3, -2, -4},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 4,  2,  3,  5,  6,  3,  2,  4}
};

bool    isWhiteTurn   = true;
int8_t  selX = -1, selY = -1;
int8_t  epX  = -1, epY  = -1;
uint8_t castlingFlags = 0;
