#include "update.h"
#include "defs.h"
#include <stdio.h>  // Para printf
#include <stdlib.h> // Para rand()
#include <math.h>   // Para sqrtf e roundf

// cria UM tiro com ângulo específico
static void createSingleBullet(Player *player, Bullet bullets[], float angleOffset) {
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].rect.w = (int)(PLAYER_BULLET_SIZE * 0.6);
            bullets[i].rect.h = PLAYER_BULLET_SIZE;
            
            bullets[i].rect.x = player->rect.x + (player->rect.w / 2) - (bullets[i].rect.w / 2);
            bullets[i].rect.y = player->rect.y + (player->rect.h / 2) - (bullets[i].rect.h / 2);

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Vetor direção base
            float dirX = (float)mouseX - (bullets[i].rect.x + bullets[i].rect.w / 2.0f);
            float dirY = (float)mouseY - (bullets[i].rect.y + bullets[i].rect.h / 2.0f);
            
            float cosA = cosf(angleOffset);
            float sinA = sinf(angleOffset);
            
            float rotDirX = dirX * cosA - dirY * sinA;
            float rotDirY = dirX * sinA + dirY * cosA;

            float len = sqrtf(rotDirX * rotDirX + rotDirY * rotDirY);

            // velocidade final com o multiplicador do player
            float finalSpeed = PLAYER_BULLET_SPEED * player->bullet_speed_mult;

            if (len == 0) {
                bullets[i].dx = finalSpeed;
                bullets[i].dy = 0;
            } else {
                bullets[i].dx = (rotDirX / len) * finalSpeed;
                bullets[i].dy = (rotDirY / len) * finalSpeed;
            }
            break; 
        }
    }
}

// criar um novo projétil
static void shootBullet(Player *player, Bullet bullets[]) {
    // Tiro central (sempre acontece)
    createSingleBullet(player, bullets, 0.0f);

    // Multishot
    if (player->multishot_level >= 1) {
        // 2 tiros extras
        createSingleBullet(player, bullets, -0.15f);
        createSingleBullet(player, bullets, 0.15f); 
    }
    if (player->multishot_level >= 2) {
        // +2 tiros extras (total 5)
        createSingleBullet(player, bullets, -0.30f); 
        createSingleBullet(player, bullets, 0.30f);
    }
}

// cria um novo projétil da cafeína
static void enemyShootBullet(Enemy *enemy, Bullet enemyBullets[], Player *player) {
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (!enemyBullets[i].active) {
            enemyBullets[i].active = true;
            enemyBullets[i].rect.w = ENEMY_BULLET_SIZE;
            enemyBullets[i].rect.h = ENEMY_BULLET_SIZE;
            enemyBullets[i].rect.x = enemy->rect.x + (enemy->rect.w / 2) - (ENEMY_BULLET_SIZE / 2);
            enemyBullets[i].rect.y = enemy->rect.y + (enemy->rect.h / 2) - (ENEMY_BULLET_SIZE / 2);

            float dirX = (player->rect.x + player->rect.w / 2.0f) - (enemyBullets[i].rect.x + ENEMY_BULLET_SIZE / 2.0f);
            float dirY = (player->rect.y + player->rect.h / 2.0f) - (enemyBullets[i].rect.y + ENEMY_BULLET_SIZE / 2.0f);
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

// cria um novo inimigo em uma das bordas
static void spawnEnemy(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            
            enemies[i].type = rand() % 2; // 0 ou 1
            int scaleFactor = 1;

            if (enemies[i].type == 0) {
                // ANSIEDADE
                enemies[i].hp = ANXIETY_STARTING_HP;
                enemies[i].points_given = ANXIETY_POINTS_GIVEN;
                enemies[i].xp_given = ANXIETY_XP_GIVEN;
                enemies[i].speed = ANXIETY_SPEED;
                scaleFactor = ANXIETY_SCALE_FACTOR;
            } else {
                // CAFEÍNA
                enemies[i].hp = CAFEINA_STARTING_HP;
                enemies[i].points_given = CAFEINA_POINTS_GIVEN;
                enemies[i].xp_given = CAFEINA_XP_GIVEN;
                enemies[i].speed = CAFEINA_SPEED;
                enemies[i].lastShotTime = SDL_GetTicks();
                scaleFactor = CAFEINA_SCALE_FACTOR;
            }

            // define o tamanho baseado na escala
            enemies[i].rect.w = ENEMY_ORIGINAL_SIZE * scaleFactor;
            enemies[i].rect.h = ENEMY_ORIGINAL_SIZE * scaleFactor;
            // ---------------------------------------

            // define a posição aleatória
            int edge = rand() % 4;
            switch (edge) {
                case 0: // Topo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - enemies[i].rect.w);
                    enemies[i].rect.y = -enemies[i].rect.h;
                    break;
                case 1: // Baixo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - enemies[i].rect.w);
                    enemies[i].rect.y = SCREEN_HEIGHT;
                    break;
                case 2: // Esquerda
                    enemies[i].rect.x = -enemies[i].rect.w;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - enemies[i].rect.h);
                    break;
                case 3: // Direita
                    enemies[i].rect.x = SCREEN_WIDTH;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - enemies[i].rect.h);
                    break;
            }
            break;
        }
    }
}

// lógica de separação de inimigos
static void separateEnemies(Enemy enemies[], int enemyCount) {
    SDL_Rect overlap;
    for (int i = 0; i < enemyCount; i++) {
        for (int j = i + 1; j < enemyCount; j++) {
            if (SDL_HasIntersection(&enemies[i].rect, &enemies[j].rect)) {
                SDL_IntersectRect(&enemies[i].rect, &enemies[j].rect, &overlap);
                if (overlap.w > MAX_OVERLAP || overlap.h > MAX_OVERLAP) {
                    if (overlap.w < overlap.h) {
                        if (enemies[i].rect.x < enemies[j].rect.x) {
                            enemies[i].rect.x -= (int)(overlap.w * 0.3 * SEPARATION_STRENGTH);
                            enemies[j].rect.x += (int)(overlap.w * 0.3 * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.x += (int)(overlap.w * 0.3 * SEPARATION_STRENGTH);
                            enemies[j].rect.x -= (int)(overlap.w * 0.3 * SEPARATION_STRENGTH);
                        }
                    } else {
                        if (enemies[i].rect.y < enemies[j].rect.y) {
                            enemies[i].rect.y -= (int)(overlap.h * 0.3 * SEPARATION_STRENGTH);
                            enemies[j].rect.y += (int)(overlap.h * 0.3 * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.y += (int)(overlap.h * 0.3 * SEPARATION_STRENGTH);
                            enemies[j].rect.y -= (int)(overlap.h * 0.3 * SEPARATION_STRENGTH);
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
void update(Player *player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], bool *running, Uint32 *lastEnemySpawnTime, GameState *currentState, App *app) {
    // 1. Atualizar Jogador
    player->rect.x += player->dx;
    player->rect.y += player->dy;

    if (player->dx > 0) { // Movendo para a Direita
        player->frameX = 0; 
    } else if (player->dx < 0) { // Movendo para a Esquerda
        player->frameX = 32;
    } else if (player->dy < 0) { // Movendo para Cima
        player->frameX = 64;
    } else if (player->dy > 0) { // Movendo para Baixo
        player->frameX = 96;
    }

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
    
     // LÓGICA DE DIFICULDADE BASEADA NO TEMPO ---
    
    // Exemplo: Aumentar spawn rate na metade do tempo (2.5 minutos)
    Uint32 timeLimitThreshold = ROUND_DURATION_MS / 2;
    int currentSpawnInterval = ENEMY_SPAWN_INTERVAL; // 800ms
    
    if (app->elapsedTime > timeLimitThreshold) {
        currentSpawnInterval = ENEMY_SPAWN_INTERVAL / 2; // 400ms - Dobra a velocidade de spawn
        // Você também pode mudar as cores de fundo ou forçar um tipo de inimigo aqui
    }
    
    // 3. Spawns de Inimigos
    if (now > *lastEnemySpawnTime + currentSpawnInterval) {
        spawnEnemy(enemies);
        *lastEnemySpawnTime = now;
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
    
    // Bullet vs Inimigo
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) continue;
            if (SDL_HasIntersection(&bullets[i].rect, &enemies[j].rect)) {
                bullets[i].active = false; 
                
                // Aplica dano com multiplicador
                // Convertemos o float damage para int (arredondando para baixo, minimo 1)
                int damageDealt = (int)player->damage;
                if (damageDealt < 1) damageDealt = 1;

                enemies[j].hp -= damageDealt;

                if (enemies[j].hp <= 0) {
                    enemies[j].active = false; 
                    player->points += enemies[j].points_given;
                    player->xp += enemies[j].xp_given;
                }
            }
        }
    }

    // Player vs Inimigo (Corpo a corpo)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (!player->invulnerable && SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                
                // --- LÓGICA DE ESQUIVA ---
                float chance = (float)rand() / (float)RAND_MAX;
                if (chance < player->dodge_chance) {
                    printf("ESQUIVA! (Chance: %.2f)\n", player->dodge_chance);
                    // Torna invulnerável brevemente sem tirar vida
                    player->invulnerable = true;
                    player->invulnerableTime = SDL_GetTicks();
                    // Pequeno empurrão visual ou apenas ignora o dano
                } else {
                    player->hp--;
                    printf("Dano recebido! HP: %d\n", player->hp);
                    player->invulnerable = true;
                    player->invulnerableTime = SDL_GetTicks();
                    
                    if (player->hp <= 0) {
                        *currentState = STATE_END;
                    } 
                }
                
                // Inimigo também toma dano ao bater
                int damageDealt = (int)player->damage;
                if (damageDealt < 1) damageDealt = 1;
                enemies[i].hp -= damageDealt;

                if (enemies[i].hp <= 0) {
                    enemies[i].active = false;
                    player->points += enemies[i].points_given;
                    player->xp += enemies[i].xp_given;
                }
                break; 
            }
        }
    }
    
    // Player vs Bullet Inimiga
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (!enemyBullets[i].active) continue;
        if (!player->invulnerable && SDL_HasIntersection(&enemyBullets[i].rect, &player->rect)) {
            
            // --- LÓGICA DE ESQUIVA ---
            float chance = (float)rand() / (float)RAND_MAX;
            if (chance < player->dodge_chance) {
                 printf("ESQUIVA DE PROJETIL!\n");
                 enemyBullets[i].active = false; // Bala passa direto (some)
            } else {
                player->hp -= 1;
                player->invulnerable = true;
                player->invulnerableTime = SDL_GetTicks();
                enemyBullets[i].active = false;

                if (player->hp <= 0) {
                    *currentState = STATE_END;
                }
            }
        }
    }
    
    // Timer
    app->elapsedTime = SDL_GetTicks() - app->roundStartTime;

    if (app->elapsedTime >= ROUND_DURATION_MS) {
        // FIM DE JOGO (VITÓRIA)
        *currentState = STATE_END;
        return;
    }

    //Calcula o número de degraus (steps) de escuridão concluídos
    int completedSteps = app->elapsedTime / DARKNESS_STEP_MS;
    
    float alphaPerStep = (float)MAX_MAX_DARKNESS_ALPHA / (ROUND_DURATION_MS / DARKNESS_STEP_MS); 

    int targetAlpha = (int)(completedSteps * alphaPerStep);

    if (targetAlpha > MAX_MAX_DARKNESS_ALPHA) {
        targetAlpha = MAX_MAX_DARKNESS_ALPHA;
    }

    app->currentDarknessAlpha = targetAlpha;
    
    if (app->elapsedTime >= ROUND_DURATION_MS) {
        // FIM DE JOGO
        *currentState = STATE_END;
        return;
    }

}
