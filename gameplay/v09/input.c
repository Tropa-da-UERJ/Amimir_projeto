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

// Função auxiliar para aplicar o upgrade
static void applyUpgrade(Player *player, UpgradeType type) {
    switch (type) {
        case UPGRADE_HP_MAX:
            player->max_hp += 5;
            player->hp = player->max_hp; // Cura total ao upar vida
            printf("Upgrade: HP Maximo +5\n");
            break;
        case UPGRADE_FIRE_RATE:
            player->fire_delay *= 0.9f; // -10% delay
            printf("Upgrade: Cadencia de Tiro +10%%\n");
            break;
        case UPGRADE_MOVE_SPEED:
            player->speed += 1;
            printf("Upgrade: Velocidade +1\n");
            break;
        case UPGRADE_DAMAGE:
            player->damage *= 1.1f; // +10% multiplicativo
            printf("Upgrade: Dano Base +10%% (Atual: %.2f)\n", player->damage);
            break;
        case UPGRADE_MULTISHOT:
            player->multishot_level++;
            printf("Upgrade: Multishot Nivel %d\n", player->multishot_level);
            break;
        case UPGRADE_BULLET_SPEED:
            player->bullet_speed_mult += 0.15f;
            printf("Upgrade: Velocidade do Projetil +15%%\n");
            break;
        case UPGRADE_DODGE:
            player->dodge_chance += 0.10f; // +10% chance de esquiva
            if (player->dodge_chance > 0.60f) player->dodge_chance = 0.60f; // Cap em 60%
            printf("Upgrade: Esquiva +10%% (Total: %.0f%%)\n", player->dodge_chance * 100);
            break;
        default:
            break;
    }
}

void handleLevelUpInput (SDL_Event *event, Player *player, GameState *currentState, bool *running, App *app) {
     if (event->type == SDL_KEYDOWN) {
        int selectedIndex = -1;
        
        switch (event->key.keysym.sym) {
            case SDLK_1:
            case SDLK_KP_1:
                selectedIndex = 0;
                break;
            case SDLK_2:
            case SDLK_KP_2:
                selectedIndex = 1;
                break;
            case SDLK_3:
            case SDLK_KP_3:
                selectedIndex = 2;
                break;
        }

        if (selectedIndex != -1) {
            // Aplica o upgrade correspondente ao índice sorteado
            applyUpgrade(player, app->currentUpgradeOptions[selectedIndex]);
            *currentState = STATE_PLAYING;
        }
    }
}

