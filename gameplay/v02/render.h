#ifndef RENDER_H
#define RENDER_H

#include "defs.h"

void render(SDL_Renderer *renderer, App *app, Player player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], GameState currentState);

#endif // RENDER_H
