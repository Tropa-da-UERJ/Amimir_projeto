#ifndef INIT_H
#define INIT_H

#include "defs.h" // Inclui as definições de struct

void initPlayer(Player *player);
void initEnemies(Enemy enemies[]);
void initBullets(Bullet bullets[]);
void initEnemyBullets(Bullet enemyBullets[]); // Renomeado de eBullets para clareza

#endif // INIT_H
