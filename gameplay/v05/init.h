#ifndef INIT_H
#define INIT_H

#include "defs.h" // Inclui as definições de struct
#include <SDL2/SDL.h>

void initPlayer(Player *player, SDL_Renderer *renderer);
void initEnemies(Enemy enemies[]);
void initBullets(Bullet bullets[]);
void initEnemyBullets(Bullet enemyBullets[]); // Renomeado de eBullets para clareza

#endif // INIT_H
