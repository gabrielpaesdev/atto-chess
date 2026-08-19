#pragma once
#include <cstdint>

// TILE_SIZE potência de 2 → shifts em vez de multiplicação
constexpr int8_t TILE_SHIFT  = 6;
constexpr int8_t TILE_SIZE   = 1 << TILE_SHIFT;   // 64
constexpr int8_t BOARD_SIZE  = 8;
constexpr int    WINDOW_SIZE = TILE_SIZE * BOARD_SIZE;

// int8_t enum: 1 byte por valor
enum PieceType : int8_t {
    EMPTY = 0, PAWN = 1, KNIGHT = 2, BISHOP = 3,
    ROOK  = 4, QUEEN = 5, KING   = 6
};

// Inline: zero-overhead; elimina <cmath>
inline int8_t abs8(int8_t x) { return x < 0 ? -x : x; }
