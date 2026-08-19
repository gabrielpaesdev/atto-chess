#include "moves.h"
#include "board.h"

bool isPathClear(int8_t startX, int8_t startY, int8_t endX, int8_t endY) {
    int8_t dx = (endX > startX) - (endX < startX);
    int8_t dy = (endY > startY) - (endY < startY);
    int8_t x  = startX + dx;
    int8_t y  = startY + dy;

    while (x != endX || y != endY) {
        if (board[y][x] != EMPTY) return false;
        x += dx;
        y += dy;
    }
    return true;
}

bool isPseudoLegalMove(int8_t startX, int8_t startY, int8_t endX, int8_t endY) {
    int8_t piece  = board[startY][startX];
    int8_t target = board[endY][endX];

    if (target != EMPTY && ((piece > 0) == (target > 0))) return false;

    int8_t type = abs8(piece);
    int8_t dx   = abs8(endX - startX);
    int8_t dy   = abs8(endY - startY);
    int8_t dir  = (piece > 0) ? -1 : 1;

    switch (type) {
        case PAWN:
            if (dx == 0 && endY - startY == dir && target == EMPTY) return true;

            if (dx == 0 && endY - startY == (dir << 1) &&
                startY == (piece > 0 ? 6 : 1) &&
                target == EMPTY && board[startY + dir][startX] == EMPTY) return true;

            if (dx == 1 && endY - startY == dir &&
                (target != EMPTY || (endX == epX && endY == epY))) return true;
            return false;

        case KNIGHT:
            return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);

        case BISHOP:
            if (dx != dy) return false;
            return isPathClear(startX, startY, endX, endY);

        case ROOK:
            if (dx != 0 && dy != 0) return false;
            return isPathClear(startX, startY, endX, endY);

        case QUEEN:
            if (dx != 0 && dy != 0 && dx != dy) return false;
            return isPathClear(startX, startY, endX, endY);

        case KING:
            if (dx <= 1 && dy <= 1) return true;

            if (dy == 0 && dx == 2) {
                if (piece > 0 && (castlingFlags & 1) && startY == 7 && startX == 4) {
                    if (endX == 6 && (castlingFlags & 4) && isPathClear(4, 7, 7, 7)) return true;
                    if (endX == 2 && (castlingFlags & 2) && isPathClear(4, 7, 0, 7)) return true;
                } else if (piece < 0 && (castlingFlags & 8) && startY == 0 && startX == 4) {
                    if (endX == 6 && (castlingFlags & 32) && isPathClear(4, 0, 7, 0)) return true;
                    if (endX == 2 && (castlingFlags & 16) && isPathClear(4, 0, 0, 0)) return true;
                }
            }
            return false;
    }
    return false;
}
