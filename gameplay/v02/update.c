#include "update.h"
#include <stdio.h>  // Para printf
#include <stdlib.h> // Para rand()
#include <math.h>   // Para sqrtf e roundf

// --- Funções Auxiliares (privadas para este módulo) ---

/**
 * Tenta criar um novo projétil. (Chamada por update)
 */
static void shootBullet(Player *player, Bullet bullets[]) {
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].rect.w = BULLET_SIZE;
            bullets[i].rect.h = BULLET_SIZE;
            bullets[i].rect.x = player->rect.x + (player->rect.w / 2) - (bullets[i].rect.w / 2);
            bullets[i].rect.y = player->rect.y + (player->rect.h / 2) - (bullets[i].rect.h / 2);

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            float dirX = (float)mouseX - (bullets[i].rect.x + bullets[i].rect.w / 2.0f);
            float dirY = (float)mouseY - (bullets[i].rect.y + bullets[i].rect.h / 2.0f);
            float len = sqrtf(dirX * dirX + dirY * dirY);

            if (len == 0) {
                bullets[i].dx = PLAYER_BULLET_SPEED;
                bullets[i].dy = 0;
            } else {
                bullets[i].dx = (int)roundf((dirX / len) * PLAYER_BULLET_SPEED);
                bullets[i].dy = (int)roundf((dirY / len) * PLAYER_BULLET_SPEED);
            }
            break; 
        }
    }
}

/**
 * Tenta criar um novo projétil DA CAFEINA. (Chamada por update)
 */
static void enemyShootBullet(Enemy *enemy, Bullet enemyBullets[], Player *player) {
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (!enemyBullets[i].active) {
            enemyBullets[i].active = true;
            enemyBullets[i].rect.w = BULLET_SIZE;
            enemyBullets[i].rect.h = BULLET_SIZE;
            enemyBullets[i].rect.x = enemy->rect.x + (enemy->rect.w / 2) - (BULLET_SIZE / 2);
            enemyBullets[i].rect.y = enemy->rect.y + (enemy->rect.h / 2) - (BULLET_SIZE / 2);

            float dirX = (player->rect.x + player->rect.w / 2.0f) - (enemyBullets[i].rect.x + BULLET_SIZE / 2.0f);
            float dirY = (player->rect.y + player->rect.h / 2.0f) - (enemyBullets[i].rect.y + BULLET_SIZE / 2.0f);
            float len = sqrtf(dirX * dirX + dirY * dirY);
            float speed = CAFEINA_BULLET_SPEED; 

            if (len != 0) {
                enemyBullets[i].dx = (dirX / len) * speed;
                enemyBullets[i].dy = (dirY / len) * speed;
            } else {
                enemyBullets[i].dx = 0;
                enemyBullets[i].dy = 0;
            }
            break;
        }
    }
}

/**
 * Tenta criar um novo inimigo em uma das bordas. (Chamada por update)
 */
static void spawnEnemy(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].rect.w = SQUARE_SIZE;
            enemies[i].rect.h = SQUARE_SIZE;
            
            if (enemies[i].type == 0) {
              enemies[i].hp = ANXIETY_STARTING_HP;
            } else {
              enemies[i].hp = CAFEINA_STARTING_HP;
              enemies[i].lastShotTime = 0;
            }

            int edge = rand() % 4;
            switch (edge) {
                case 0: // Topo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - SQUARE_SIZE);
                    enemies[i].rect.y = 0 - SQUARE_SIZE;
                    break;
                case 1: // Baixo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - SQUARE_SIZE);
                    enemies[i].rect.y = SCREEN_HEIGHT;
                    break;
                case 2: // Esquerda
                    enemies[i].rect.x = 0 - SQUARE_SIZE;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - SQUARE_SIZE);
                    break;
                case 3: // Direita
                    enemies[i].rect.x = SCREEN_WIDTH;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - SQUARE_SIZE);
                    break;
            }
            break; 
        }
    }
}

/**
 * Lógica de separação de inimigos. (Chamada por update)
 */
static void separateEnemies(Enemy enemies[], int enemyCount) {
    SDL_Rect overlap;
    for (int i = 0; i < enemyCount; i++) {
        for (int j = i + 1; j < enemyCount; j++) {
            if (SDL_HasIntersection(&enemies[i].rect, &enemies[j].rect)) {
                SDL_IntersectRect(&enemies[i].rect, &enemies[j].rect, &overlap);
                if (overlap.w > MAX_OVERLAP || overlap.h > MAX_OVERLAP) {
                    if (overlap.w < overlap.h) {
                        if (enemies[i].rect.x < enemies[j].rect.x) {
                            enemies[i].rect.x -= (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.x += (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.x += (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.x -= (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                        }
                    } else {
                        if (enemies[i].rect.y < enemies[j].rect.y) {
                            enemies[i].rect.y -= (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.y += (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.y += (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.y -= (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                        }
                    }
                }
            }
        }
    }
}


/**
 * Atualiza o estado de todos os objetos do jogo. (Função PÚBLICA)
 */
void update(Player *player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], bool *running, Uint32 *lastEnemySpawnTime) {
    // 1. Atualizar Jogador
    player->rect.x += player->dx;
    player->rect.y += player->dy;

    Uint32 now = SDL_GetTicks();
        if (now - player->lastShotTime >= player->fire_delay) {
        shootBullet(player, bullets);
        player->lastShotTime = now;
        }

    // Manter jogador dentro da tela
    if (player->rect.x < 0) player->rect.x = 0;
    if (player->rect.y < 0) player->rect.y = 0;
    if (player->rect.x > SCREEN_WIDTH - player->rect.w) player->rect.x = SCREEN_WIDTH - player->rect.w;
    if (player->rect.y > SCREEN_HEIGHT - player->rect.h) player->rect.y = SCREEN_HEIGHT - player->rect.h;

    // Invulnerabilidade do player
    if (player->invulnerable) {
    Uint32 now_inv = SDL_GetTicks(); // 'now' já foi declarado acima
        if (now_inv - player->invulnerableTime >= INVULNERABILITY_DURATION) {
        player->invulnerable = false;
        }
    }

    // 2. Atualizar Projéteis
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].rect.x += bullets[i].dx;
            bullets[i].rect.y += bullets[i].dy;
            if (bullets[i].rect.x < 0 || bullets[i].rect.x > SCREEN_WIDTH ||
                bullets[i].rect.y < 0 || bullets[i].rect.y > SCREEN_HEIGHT) {
                bullets[i].active = false;
            }
        }
    }
    // projeteis inimigos
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (enemyBullets[i].active) {
            enemyBullets[i].rect.x += enemyBullets[i].dx;
            enemyBullets[i].rect.y += enemyBullets[i].dy;
            if (enemyBullets[i].rect.x < 0 || enemyBullets[i].rect.x > SCREEN_WIDTH ||
                enemyBullets[i].rect.y < 0 || enemyBullets[i].rect.y > SCREEN_HEIGHT) {
                enemyBullets[i].active = false;
            }
          }
    }
    
    // 3. Spawns de Inimigos
    Uint32 currentTime = SDL_GetTicks(); // 'now' já foi declarado acima
    if (currentTime - *lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL) {
        spawnEnemy(enemies);
        *lastEnemySpawnTime = currentTime;
    }

    // 4. Atualizar Inimigos (Perseguição)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        separateEnemies(enemies, MAX_ENEMIES);
        if (enemies[i].active) {
            if (enemies[i].type == 0) {
            // ANSIEDADE
            if (enemies[i].rect.x < player->rect.x) enemies[i].rect.x += enemies[i].speed;
            if (enemies[i].rect.x > player->rect.x) enemies[i].rect.x -= enemies[i].speed;
            if (enemies[i].rect.y < player->rect.y) enemies[i].rect.y += enemies[i].speed;
            if (enemies[i].rect.y > player->rect.y) enemies[i].rect.y -= enemies[i].speed;
            } else {
            // CAFEINA
            float dx = (player->rect.x + player->rect.w / 2.0f) - (enemies[i].rect.x + enemies[i].rect.w / 2.0f);
            float dy = (player->rect.y + player->rect.h / 2.0f) - (enemies[i].rect.y + enemies[i].rect.h / 2.0f);
            float dist = sqrtf(dx * dx + dy * dy);

              if (dist > CAFEINA_STOP_DISTANCE) {
                enemies[i].rect.x += (int)((dx / dist) * 1.5f);
                enemies[i].rect.y += (int)((dy / dist) * 1.5f);
              }

              if (currentTime - enemies[i].lastShotTime >= CAFEINA_FIRE_DELAY) {
                enemyShootBullet(&enemies[i], enemyBullets, player);
                enemies[i].lastShotTime = currentTime;
              }
            }
         }
    }

    // 5. Detecção de Colisão
    
    // Projétil vs Inimigo
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) continue;
            if (SDL_HasIntersection(&bullets[i].rect, &enemies[j].rect)) {
                bullets[i].active = false; 
                enemies[j].hp-= 1;
                if (enemies[j].hp <= 0) {
                    enemies[j].active = false; 
                    player->points += enemies[j].points_given;
                    player->xp += enemies[j].xp_given;
                }
            }
        }
    }

    // Jogador vs Inimigo
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (!player->invulnerable && SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                player->hp--;
                printf("Vida perdida! Vidas restantes: %d\n", player->hp);
                player->invulnerable = true;
                player->invulnerableTime = SDL_GetTicks();
                enemies[i].hp-= 1;

                if (player->hp <= 0) {
                    printf("GAME OVER! Você não tem mais vidas.\n");
                    printf("Você fez %d pontos e acumulou %d xp!\n", player->points, player->xp);
                    *running = false;
                } 
                if (enemies[i].hp <= 0) {
                    enemies[i].active = false;
                    player->points += enemies[i].points_given;
                    player->xp += enemies[i].xp_given;
                }
                break; 
            }
            else if (player->invulnerable && SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                enemies[i].hp-= 1;
                if (enemies[i].hp <= 0) {
                    enemies[i].active = false; 
                    player->points += enemies[i].points_given;
                    player->xp += enemies[i].xp_given;
               }
                break; 
            }
        }
    }
    
    // Jogador vs Projetil Inimigo
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (!enemyBullets[i].active) continue;
        if (!player->invulnerable && SDL_HasIntersection(&enemyBullets[i].rect, &player->rect)) {
            player->hp -= 1;
            printf("Vida perdida! Vidas restantes: %d\n", player->hp);
            player->invulnerable = true;
            player->invulnerableTime = SDL_GetTicks();
            enemyBullets[i].active = false;

            if (player->hp <= 0) {
                printf("GAME OVER! Você não tem mais vidas.\n");
                printf("Você fez %d pontos e acumulou %d xp!\n", player->points, player->xp);
                *running = false;
            }
        }
    }
}
