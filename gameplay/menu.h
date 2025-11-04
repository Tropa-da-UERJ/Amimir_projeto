#pragma once
#include <SDL.h>
#include "game.h" // Inclui GameState

typedef struct {
    SDL_Rect rect;
    SDL_Texture* texNormal;
    SDL_Texture* texSelected;
    bool isSelected;
    GameState targetState; 
} Button;

void initMenu(SDL_Renderer* renderer);
void handleMenuInput(SDL_Event* event, GameState* currentState);
void renderMenu(SDL_Renderer* renderer);
void cleanupMenu();
