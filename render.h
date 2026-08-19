#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

void loadPieceTextures(SDL_Renderer* renderer);
void freePieceTextures();
void drawGame(SDL_Renderer* renderer);
void drawText(SDL_Renderer* renderer, const char* text, int x, int y, int scale = 2);

#endif
