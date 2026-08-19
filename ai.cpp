#include "ai.h"
#include "board.h"
#include "game.h"
#include <cstring>
#include <cstdlib>

static int maxDepth = 2;
static int errorChance = 40;
static int randomNoise = 200;

static uint32_t rng_state = 2463534242;

static uint32_t xorshift32() {
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

void setAIDifficulty(AIDifficulty diff) {
    switch (diff) {
        case DIFF_BEGINNER:
            maxDepth = 2; errorChance = 40; randomNoise = 200; break;
        case DIFF_INTERMEDIATE:
            maxDepth = 4; errorChance = 1; randomNoise = 50; break;
        case DIFF_ADVANCED:
            maxDepth = 7; errorChance = 0; randomNoise = 0; break;
    }
}

static inline int getPieceValue(int8_t type) {
    switch (type) {
        case 1: return 100;
        case 2: return 320;
        case 3: return 340;
        case 4: return 500;
        case 5: return 900;
        case 6: return 20000;
        default: return 0;
    }
}

static int evaluateBoard() {
    int score = 0;
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int8_t p = board[y][x];
            if (p == EMPTY) continue;
            int val = getPieceValue(abs8(p));
            score += (p > 0) ? val : -val;
        }
    }
    if (randomNoise > 0) {
        int noise = (xorshift32() % (randomNoise * 2)) - randomNoise;
        score += noise;
    }
    return score;
}

struct MoveList {
    Move moves[128];
    int count = 0;
};

static void tryMove(MoveList& list, int8_t sx, int8_t sy, int8_t dx, int8_t dy) {
    if (dx >= 0 && dx < BOARD_SIZE && dy >= 0 && dy < BOARD_SIZE) {
        if (isLegalMove(sx, sy, dx, dy)) {
            int attacker = getPieceValue(abs8(board[sy][sx]));
            int victim = getPieceValue(abs8(board[dy][dx]));
            
            int16_t score = 0;
            if (victim > 0) {
                score = (victim * 10) - attacker;
            }
            list.moves[list.count++] = {sx, sy, dx, dy, score};
        }
    }
}

static void sortMoves(MoveList& list) {
    for (int i = 1; i < list.count; i++) {
        Move key = list.moves[i];
        int j = i - 1;
        while (j >= 0 && list.moves[j].score < key.score) {
            list.moves[j + 1] = list.moves[j];
            j--;
        }
        list.moves[j + 1] = key;
    }
}

static void generateMoves(MoveList& list, bool isWhite) {
    list.count = 0;
    int8_t dir = isWhite ? -1 : 1;
    
    for (int sy = 0; sy < BOARD_SIZE; ++sy) {
        for (int sx = 0; sx < BOARD_SIZE; ++sx) {
            int8_t p = board[sy][sx];
            if (p == EMPTY || ((p > 0) != isWhite)) continue;
            
            int8_t type = abs8(p);
            
            if (type == 1) {
                tryMove(list, sx, sy, sx, sy + dir);
                tryMove(list, sx, sy, sx, sy + dir * 2);
                tryMove(list, sx, sy, sx - 1, sy + dir);
                tryMove(list, sx, sy, sx + 1, sy + dir);
            } 
            else if (type == 2) {
                static const int8_t kx[] = {1, 2, 2, 1, -1, -2, -2, -1};
                static const int8_t ky[] = {-2, -1, 1, 2, 2, 1, -1, -2};
                for (int i = 0; i < 8; i++) tryMove(list, sx, sy, sx + kx[i], sy + ky[i]);
            }
            else if (type == 6) {
                static const int8_t kx[] = {0, 1, 1, 1, 0, -1, -1, -1};
                static const int8_t ky[] = {-1, -1, 0, 1, 1, 1, 0, -1};
                for (int i = 0; i < 8; i++) tryMove(list, sx, sy, sx + kx[i], sy + ky[i]);
            }
            else {
                if (type == 4 || type == 5) {
                    for (int i = 1; i < BOARD_SIZE; i++) {
                        tryMove(list, sx, sy, sx, sy + i);
                        tryMove(list, sx, sy, sx, sy - i);
                        tryMove(list, sx, sy, sx + i, sy);
                        tryMove(list, sx, sy, sx - i, sy);
                    }
                }
                if (type == 3 || type == 5) {
                    for (int i = 1; i < BOARD_SIZE; i++) {
                        tryMove(list, sx, sy, sx + i, sy + i);
                        tryMove(list, sx, sy, sx - i, sy - i);
                        tryMove(list, sx, sy, sx + i, sy - i);
                        tryMove(list, sx, sy, sx - i, sy + i);
                    }
                }
            }
        }
    }
    sortMoves(list);
}

static int minimax(int depth, int alpha, int beta, bool isMaximizing) {
    if (depth == 0) return evaluateBoard();

    MoveList list;
    generateMoves(list, isMaximizing);

    if (list.count == 0) {
        return isMaximizing ? -30000 : 30000;
    }

    int8_t savedBoard[BOARD_SIZE][BOARD_SIZE];
    
    if (isMaximizing) {
        int maxEval = -2000000;
        for (int i = 0; i < list.count; ++i) {
            memcpy(savedBoard, board, sizeof(board)); 
            
            executeMove(list.moves[i].fromX, list.moves[i].fromY, list.moves[i].toX, list.moves[i].toY);
            int eval = minimax(depth - 1, alpha, beta, false);
            
            memcpy(board, savedBoard, sizeof(board)); 

            if (eval > maxEval) maxEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 2000000;
        for (int i = 0; i < list.count; ++i) {
            memcpy(savedBoard, board, sizeof(board));
            
            executeMove(list.moves[i].fromX, list.moves[i].fromY, list.moves[i].toX, list.moves[i].toY);
            int eval = minimax(depth - 1, alpha, beta, true);
            
            memcpy(board, savedBoard, sizeof(board));

            if (eval < minEval) minEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move getBestMove(bool isWhite) {
    MoveList list;
    generateMoves(list, isWhite);

    if (errorChance > 0 && (xorshift32() % 1000) < (uint32_t)errorChance) {
        if (list.count > 0) {
            return list.moves[xorshift32() % list.count];
        }
    }

    Move bestMove = list.moves[0];
    int bestVal = isWhite ? -2000000 : 2000000;
    int8_t savedBoard[BOARD_SIZE][BOARD_SIZE];

    for (int i = 0; i < list.count; ++i) {
        memcpy(savedBoard, board, sizeof(board));
        executeMove(list.moves[i].fromX, list.moves[i].fromY, list.moves[i].toX, list.moves[i].toY);
        
        int moveVal = minimax(maxDepth - 1, -2000000, 2000000, !isWhite);
        
        memcpy(board, savedBoard, sizeof(board));

        if (isWhite) {
            if (moveVal > bestVal) {
                bestVal = moveVal;
                bestMove = list.moves[i];
            }
        } else {
            if (moveVal < bestVal) {
                bestVal = moveVal;
                bestMove = list.moves[i];
            }
        }
    }

    return bestMove;
}
