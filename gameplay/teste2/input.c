#include "input.h"

void handleInput(Player *player, bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                // Tiro automático já está em update(), 
                // então removi a tecla ESPAÇO para evitar duplicidade.
                // case SDLK_SPACE:
                //     shootBullet(player, bullets); // Requereria 'bullets' como parâmetro
                //     break;
                case SDLK_ESCAPE:
                    *running = false;
                    break;
            }
        }
    }

    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    
    player->dx = 0;
    player->dy = 0;

    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        player->dx -= PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        player->dx += PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
        player->dy -= PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
        player->dy += PLAYER_SPEED;
    }
}
