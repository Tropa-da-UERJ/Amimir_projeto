#pragma once
#include <SDL.h>
#include "game.h" // Se o placar precisar de GameState ou Player/Score

void initScoreboard(SDL_Renderer* renderer);
void updateScoreboard(void);
void renderScoreboard(SDL_Renderer* renderer);
void handleScoreboardInput(SDL_Event* event, GameState* currentState);
void cleanupScoreboard(void);
