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
                handleMenuInput(&event, &currentState);
            } else if (currentState == GAME_STATE_PLAYING) {
                handleGameInput(&event);
            }
        }

        // Limpa a tela
        SDL_RenderClear(renderer);

        // Renderização por estado
        if (currentState == GAME_STATE_MENU) {
            // updateMenu() // (Se tiver animações)
            renderMenu(renderer);
        } else if (currentState == GAME_STATE_PLAYING) {
            updateGame();
            renderGame(renderer);
        } else if (currentState == GAME_STATE_SCOREBOARD) {
            updateScoreboard(); 
            renderScoreboard(renderer);
        } else if (currentState == GAME_STATE_QUIT) {
            isRunning = false; // Quebra o loop
        }

        // Apresenta na tela
        SDL_RenderPresent(renderer);
    }

    // Limpa os recursos
    cleanupMenu();
    cleanupGame();

    return 0;
}
