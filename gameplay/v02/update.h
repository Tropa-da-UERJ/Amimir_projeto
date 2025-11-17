#ifndef UPDATE_H
#define UPDATE_H

#include "defs.h"

void update(Player *player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], bool *running, Uint32 *lastEnemySpawnTime);

#endif // UPDATE_H
