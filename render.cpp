#include "render.h"
#include "board.h"
#include "font8x8.h"
#include "game.h"
#include "piece.h"
#include <cstdlib>

static SDL_Texture* whitePieces[7];
static SDL_Texture* blackPieces[7];

static void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

void loadPieceTextures(SDL_Renderer* renderer) {
    for (int i = 1; i <= 6; ++i) {
        SDL_Surface* blackSurface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_RGBA32);
        SDL_Surface* whiteSurface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_RGBA32);

        uint32_t* bPixels = (uint32_t*)blackSurface->pixels;
        uint32_t* wPixels = (uint32_t*)whiteSurface->pixels;

        for (int row = 0; row < 16; ++row) {
            for (int col = 0; col < 16; ++col) {
                int index = row * 16 + col;
                bool isSolid = (piece_sprites[i - 1][row * 2 + (col / 8)] & (0x80 >> (col % 8)));

                if (isSolid) {
                    bPixels[index] = 0xFF000000;
                    wPixels[index] = 0xFFFFFFFF;
                } else {
                    bPixels[index] = 0x00000000;
                    wPixels[index] = 0x00000000;
                }
            }
        }

        blackPieces[i] = SDL_CreateTextureFromSurface(renderer, blackSurface);
        whitePieces[i] = SDL_CreateTextureFromSurface(renderer, whiteSurface);

        SDL_FreeSurface(blackSurface);
        SDL_FreeSurface(whiteSurface);
    }
}

void freePieceTextures() {
    for (int i = 1; i <= 6; ++i) {
        if (blackPieces[i]) SDL_DestroyTexture(blackPieces[i]);
        if (whitePieces[i]) SDL_DestroyTexture(whitePieces[i]);
    }
}

void drawText(SDL_Renderer* renderer, const char* text, int x, int y, int scale) {
    int cursorX = x;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = text[i];
        if (c < 32 || c > 127) c = '?';

        const unsigned char* bitmap = font8x8[c - 32];

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (bitmap[row] & (0x80 >> col)) {
                    SDL_Rect pixel = { cursorX + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursorX += 8 * scale + scale;
    }
}

void drawGame(SDL_Renderer* renderer) {
    for (int8_t y = 0; y < BOARD_SIZE; ++y) {
        for (int8_t x = 0; x < BOARD_SIZE; ++x) {
            SDL_Rect rect = { x << TILE_SHIFT, y << TILE_SHIFT, TILE_SIZE, TILE_SIZE };

            if ((x + y) & 1) SDL_SetRenderDrawColor(renderer, 181, 136, 99,  255);
            else             SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            SDL_RenderFillRect(renderer, &rect);

            if (x == selX && y == selY) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
                SDL_RenderFillRect(renderer, &rect);
            }

            int8_t piece = board[y][x];
            if (piece != EMPTY) {
                int8_t type = abs8(piece);
                SDL_Texture* tex = (piece > 0) ? whitePieces[type] : blackPieces[type];
                
                SDL_Rect pieceRect = {
                    (x << TILE_SHIFT) + 4,
                    (y << TILE_SHIFT) + 4,
                    TILE_SIZE - 8,
                    TILE_SIZE - 8
                };
                SDL_RenderCopy(renderer, tex, NULL, &pieceRect);
            }

            if (selX != -1 && isLegalMove(selX, selY, x, y)) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 127);
                int centerX = (x << TILE_SHIFT) + (TILE_SIZE >> 1);
                int centerY = (y << TILE_SHIFT) + (TILE_SIZE >> 1);
                drawFilledCircle(renderer, centerX, centerY, TILE_SIZE >> 3);
            }
        }
    }
}
