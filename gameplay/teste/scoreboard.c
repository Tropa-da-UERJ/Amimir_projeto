#include "scoreboard.h"
#include <stdio.h>

void initScoreboard(SDL_Renderer* renderer) {
    printf("DEBUG: Scoreboard: Initializing resources.\n");
}


void updateScoreboard(void) {
    printf("DEBUG: Scoreboard: Updating logic.\n");
}



void renderScoreboard(SDL_Renderer* renderer) {
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
    SDL_RenderClear(renderer);

}


void handleScoreboardInput(SDL_Event* event, GameState* currentState) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_ESCAPE || event->key.keysym.sym == SDLK_RETURN) {
            printf("DEBUG: Scoreboard: Returning to Menu.\n");
            *currentState = GAME_STATE_MENU; // Volta para o menu
        }
    }
}


void cleanupScoreboard(void) {
    printf("DEBUG: Scoreboard: Cleaning up resources.\n");

}
