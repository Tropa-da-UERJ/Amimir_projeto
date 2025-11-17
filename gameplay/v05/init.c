#include "init.h"
#include <stdlib.h> // Para rand()
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define PLAYER_SCALE_FACTOR 3

static SDL_Texture* carregarTextura(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* novaTextura = IMG_LoadTexture(renderer, path);
    if (novaTextura == NULL) {
        printf("Não foi possível carregar a textura '%s'! IMG_Error: %s\n", path, IMG_GetError());
    }
    return novaTextura;
}

void initPlayer(Player *player, SDL_Renderer *renderer) {
    player->texture = carregarTextura("assets/player_sprite.png", renderer);
    
    int frameOriginalWidth = 32;
    int frameOriginalHeight = 32;

    // APLICAÇÃO DA AMPLIAÇÃO:
    player->rect.w = frameOriginalWidth * PLAYER_SCALE_FACTOR;  // 32 * 3 = 96
    player->rect.h = frameOriginalHeight * PLAYER_SCALE_FACTOR; // 32 * 3 = 96
    
    player->rect.x = (SCREEN_WIDTH - player->rect.w) / 2;
    player->rect.y = (SCREEN_HEIGHT - player->rect.h) / 2;
    player->dx = 0;
    player->dy = 0;
    player->max_hp = PLAYER_STARTING_HP;
    player->hp = player->max_hp;
    player->invulnerable = false;
    player->invulnerableTime = 0;
    player->points = 0;
    player->xp = 0;
    player->xp_toLevel = PLAYER_XP_TOLEVEL;
    player->speed = PLAYER_SPEED;
    player->fire_delay = PLAYER_FIRE_DELAY;

    player->frameX = 0; // Inicia na posição 0
}

void initEnemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        
        enemies[i].type = rand() % 2; // 0 ou 1          
        if (enemies[i].type == 0) {
                enemies[i].points_given = ANXIETY_POINTS_GIVEN;
                enemies[i].xp_given = ANXIETY_XP_GIVEN;
                enemies[i].speed = ANXIETY_SPEED;
            } else {
                enemies[i].points_given = CAFEINA_POINTS_GIVEN;
                enemies[i].xp_given = CAFEINA_XP_GIVEN;
                enemies[i].speed = CAFEINA_SPEED;
            }
    }
}

void initBullets(Bullet bullets[]) {
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}

// Renomeado de eBullets para clareza
void initEnemyBullets(Bullet enemyBullets[]) {
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        enemyBullets[i].active = false;
    }
}
