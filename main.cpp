#include <SDL2/SDL.h>
#include <cstdio>
#include <cstring>
#include "board.h"
#include "game.h"
#include "render.h"
#include "ai.h"

enum GameState { MENU_MAIN, MENU_PLAY, MENU_DIFF, MENU_OPTIONS, MENU_CREDITS, GAME_PVP, GAME_PVE };

bool aiThinking = false;
bool aiMoveReady = false;
Move aiMove;

int computeAIMove(void* data) {
    aiMove = getBestMove(false);
    aiMoveReady = true;
    return 0;
}

int main(int, char*[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Atto Chess",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* boardSnapshot = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_SIZE, WINDOW_SIZE
    );

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    loadPieceTextures(renderer);

    bool running = true;
    SDL_Event event;

    GameState state = MENU_MAIN;
    bool isLangPT = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { running = false; break; }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx = event.button.x;
                int my = event.button.y;

                if (state == MENU_MAIN) {
                    if (my >= 200 && my <= 230) state = MENU_PLAY;
                    else if (my >= 250 && my <= 280) state = MENU_OPTIONS;
                    else if (my >= 300 && my <= 330) state = MENU_CREDITS;
                }
                else if (state == MENU_PLAY) {
                    if (my >= 200 && my <= 230) state = MENU_DIFF;
                    else if (my >= 250 && my <= 280) state = GAME_PVP;
                    else if (my >= 300 && my <= 330) state = MENU_MAIN;
                }
                else if (state == MENU_DIFF) {
                    if (my >= 150 && my <= 180) { setAIDifficulty(DIFF_BEGINNER); state = GAME_PVE; }
                    else if (my >= 200 && my <= 230) { setAIDifficulty(DIFF_INTERMEDIATE); state = GAME_PVE; }
                    else if (my >= 250 && my <= 280) { setAIDifficulty(DIFF_ADVANCED); state = GAME_PVE; }
                    else if (my >= 300 && my <= 330) state = MENU_PLAY;
                }
                else if (state == MENU_OPTIONS) {
                    if (my >= 200 && my <= 230) isLangPT = !isLangPT;
                    else if (my >= 300 && my <= 330) state = MENU_MAIN;
                }
                else if (state == MENU_CREDITS) {
                    if (my >= 300 && my <= 330) state = MENU_MAIN;
                }
                else if ((state == GAME_PVP) || (state == GAME_PVE && isWhiteTurn)) {
                    int8_t cx = mx >> TILE_SHIFT;
                    int8_t cy = my >> TILE_SHIFT;

                    if (selX == -1) {
                        int8_t p = board[cy][cx];
                        if (p != EMPTY && ((p > 0) == isWhiteTurn))
                            selX = cx, selY = cy;
                    } else if (cx == selX && cy == selY) {
                        selX = selY = -1;
                    } else if (isLegalMove(selX, selY, cx, cy)) {
                        executeMove(selX, selY, cx, cy);
                        selX = selY = -1;
                        isWhiteTurn = !isWhiteTurn;

                        if (!hasValidMoves(isWhiteTurn)) {
                            if (isKingInCheck(isWhiteTurn)) puts("Xeque-Mate!");
                            else puts("Empate!");
                        }
                    } else {
                        int8_t p = board[cy][cx];
                        if (p != EMPTY && ((p > 0) == isWhiteTurn)) selX = cx, selY = cy;
                        else selX = selY = -1;
                    }
                }
            }
        }

        if (state == GAME_PVE && !isWhiteTurn && hasValidMoves(false)) {
            if (!aiThinking && !aiMoveReady) {
                SDL_SetRenderTarget(renderer, boardSnapshot);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                drawGame(renderer);
                SDL_SetRenderTarget(renderer, nullptr);

                aiThinking = true;
                SDL_DetachThread(SDL_CreateThread(computeAIMove, "AI", nullptr));
            } else if (aiMoveReady) {
                executeMove(aiMove.fromX, aiMove.fromY, aiMove.toX, aiMove.toY);
                selX = selY = -1;
                isWhiteTurn = true;
                aiThinking = false;
                aiMoveReady = false;

                if (!hasValidMoves(isWhiteTurn)) {
                    if (isKingInCheck(isWhiteTurn)) puts("Xeque-Mate!");
                    else puts("Empate!");
                } else if (isKingInCheck(isWhiteTurn)) {
                    puts("Xeque!");
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (state == MENU_MAIN) {
            drawText(renderer, "ATTO-CHESS", 50, 50, 4);
            drawText(renderer, isLangPT ? "JOGAR" : "PLAY", 100, 200, 3);
            drawText(renderer, isLangPT ? "OPCOES" : "OPTIONS", 100, 250, 3);
            drawText(renderer, isLangPT ? "CREDITOS" : "CREDITS", 100, 300, 3);
        }
        else if (state == MENU_PLAY) {
            drawText(renderer, isLangPT ? "CONTRA MAQUINA" : "PLAY AGAINST MACHINE", 50, 200, 2);
            drawText(renderer, isLangPT ? "OFF-LINE (1V1)" : "PLAY OFF-LINE", 50, 250, 2);
            drawText(renderer, isLangPT ? "VOLTAR" : "BACK", 50, 300, 2);
        }
        else if (state == MENU_DIFF) {
            drawText(renderer, isLangPT ? "NIVEL:" : "LEVEL:", 50, 80, 3);
            drawText(renderer, isLangPT ? "1. INICIANTE" : "1. BEGINNER", 50, 150, 2);
            drawText(renderer, isLangPT ? "2. INTERMEDIARIO" : "2. INTERMEDIATE", 50, 200, 2);
            drawText(renderer, isLangPT ? "3. AVANCADO" : "3. ADVANCED", 50, 250, 2);
            drawText(renderer, isLangPT ? "VOLTAR" : "BACK", 50, 300, 2);
        }
        else if (state == MENU_OPTIONS) {
            drawText(renderer, isLangPT ? "IDIOMA: PT" : "LANGUAGE: EN", 50, 200, 2);
            drawText(renderer, isLangPT ? "VOLTAR" : "BACK", 50, 300, 2);
        }
        else if (state == MENU_CREDITS) {
            drawText(renderer, "GABRIEL PAES, 2026", 50, 200, 2);
            drawText(renderer, "<gabrielpaesdev@proton.me>", 20, 240, 2);
            drawText(renderer, "V0.1.0-BETA", 50, 270, 2);
            drawText(renderer, isLangPT ? "VOLTAR" : "BACK", 50, 300, 2);
        }
        else if (state == GAME_PVP || state == GAME_PVE) {
            if (aiThinking) {
                SDL_RenderCopy(renderer, boardSnapshot, nullptr, nullptr);
            } else {
                drawGame(renderer);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(boardSnapshot);
    freePieceTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
