#include "init.h"
#include <stdlib.h> // Para rand()

void initPlayer(Player *player) {
    player->rect.w = SQUARE_SIZE;
    player->rect.h = SQUARE_SIZE;
    player->rect.x = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
    player->rect.y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;
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
