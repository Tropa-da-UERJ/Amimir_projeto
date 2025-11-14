#include <stdio.h>
#include <SDL.h>
#include "game.h" // Inclui a lógica do jogo
#include "menu_rect.h" // Inclui a lógica do menu

int main(int argc, char* argv[]) {
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 600;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("SDL Teste de Estados", 
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // Inicializa os módulos
    //initMenu(renderer); 
    //initGame(renderer);
    initMenuRect();

    GameState currentState = GAME_STATE_MENU;
    bool game_running = true;
    SDL_Event event;

    while (game_running) {
        // Processamento de eventos centralizado ou por estado
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_running = false;
            }

            if (currentState == GAME_STATE_MENU) {
                handleMenuRectInput(&event, &currentState);
            } else if (currentState == GAME_STATE_PLAYING) {
                handleGameInput(&event);
            }
        }

        // Limpa a tela
        SDL_RenderClear(ren);

        // Renderização por estado
        if (currentState == GAME_STATE_MENU) {
            // updateMenu() // (Se tiver animações)
            renderMenuRect(ren);
        } else if (currentState == GAME_STATE_PLAYING) {
            updateGame();
            renderGame(ren);
        } else if (currentState == GAME_STATE_SCOREBOARD) {
            updateScoreboard(); 
            renderScoreboard(ren);
        } else if (currentState == GAME_STATE_QUIT) {
            game_running = false; // Quebra o loop
        }

        // Apresenta na tela
        SDL_RenderPresent(ren);
    }

    // Limpa os recursos
    cleanupMenu();
    cleanupGame();

    return 0;
}
