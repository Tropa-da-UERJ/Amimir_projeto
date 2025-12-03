#include "input.h"
#include <math.h>

void handlePlayingInput_Events(SDL_Event *event, bool *running) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                *running = false;
                break;
        }
    }
}

void handlePlayingInput_State(Player *player) {

    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    
    player->dx = 0;
    player->dy = 0;

    float dirX = 0.0f;
    float dirY = 0.0f;

    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        dirX -= 1.0f;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        dirX += 1.0f;
    }
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
        dirY -= 1.0f;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
        dirY += 1.0f;
    }

    // normalização vetorial (correção da diagonal)
    float length = sqrtf(dirX * dirX + dirY * dirY);

    if (length > 0.0f) {
        // transforma (1, 1) em (0.707, 0.707)
        dirX /= length; 
        dirY /= length;

        player->dx = dirX * player->speed;
        player->dy = dirY * player->speed;
    } else{
        // se não houver tecla pressionada (length == 0), para o player
        player->dx = 0.0f;
        player->dy = 0.0f;
    }
}

void handleLevelUpInput (SDL_Event *event, Player *player, GameState *currentState, bool *running) {
     if (event->type == SDL_KEYDOWN) {
        bool optionSelected = true;
        switch (event->key.keysym.sym) {
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

