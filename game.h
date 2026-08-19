#pragma once
#include "types.h"

bool isSquareAttacked(int8_t x, int8_t y, bool byWhite);
bool isKingInCheck(bool whiteKing);
bool isLegalMove(int8_t startX, int8_t startY, int8_t endX, int8_t endY);
bool hasValidMoves(bool whiteTurn);
void executeMove(int8_t startX, int8_t startY, int8_t endX, int8_t endY);
