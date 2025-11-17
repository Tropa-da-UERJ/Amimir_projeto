#ifndef INPUT_H
#define INPUT_H

#include "defs.h"

void handleInput(Player *player, bool *running);

void handleLevelUpInput (Player *player, GameState *currentState, bool *running);

#endif // INPUT_H
