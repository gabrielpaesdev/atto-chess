#ifndef AI_H
#define AI_H

#include <stdint.h>

enum AIDifficulty {
    DIFF_BEGINNER,
    DIFF_INTERMEDIATE,
    DIFF_ADVANCED
};

struct Move {
    int8_t fromX, fromY;
    int8_t toX, toY;
    int16_t score;
};

void setAIDifficulty(AIDifficulty diff);
Move getBestMove(bool isWhite);

#endif
