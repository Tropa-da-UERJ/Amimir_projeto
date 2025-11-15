#include "input.h"

void handleInput(Player *player, bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
               
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
        player->dx -= player->speed;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        player->dx += player->speed;
    }
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
        player->dy -= player->speed;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
        player->dy += player->speed;
    }
}

void handleLevelUpInput (Player *player, GameState *currentState, bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            bool optionSelected = true;
            switch (event.key.keysym.sym) {
                // +HP Máximo
                case SDLK_1:
                case SDLK_KP_1:
                    player->max_hp += 5;
                    player->hp = player->max_hp; //também cura
                    printf("Upgrade: HP Máximo +5! \n");
                    break;

                // +Attack Speed
                case SDLK_2:
                case SDLK_KP_2:
                    player->fire_delay = (int)(player->fire_delay * 0.9f);
                    printf("Upgrade: Cadência de tiro + 10%! \n");
                    break;

                // +Move Speed
                case SDLK_3:
                case SDLK_KP_3:
                    player->speed += 1;
                    printf("Upgrade: Velocidade de Movimento +1! \n");
                    break;
                
                default:
                    optionSelected = false; //ignora qualquer outra tecla
                    break;

            }
            if (optionSelected) {
                *currentState = STATE_PLAYING;
            }
        }
    }
}

// input.c (adicionar no final)

void handleMenuInput(GameState *currentState, bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    *running = false;
                    break;
                case SDLK_RETURN: // Tecla Enter
                case SDLK_KP_ENTER:
                    *currentState = STATE_PLAYING;
                    break;
            }
        }
    }
}

void handleEndInput(GameState *currentState, bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    *running = false;
                    break;
                case SDLK_RETURN: // Tecla Enter
                case SDLK_KP_ENTER:
                    *currentState = STATE_MENU; // Volta ao menu
                    break;
            }
        }
    }
}

