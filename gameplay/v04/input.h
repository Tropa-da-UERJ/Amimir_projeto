#ifndef INPUT_H
#define INPUT_H

#include "defs.h"

void handlePlayingInput_Events(SDL_Event *event, bool *running);
void handlePlayingInput_State(Player *player);
void handleLevelUpInput (SDL_Event *event, Player *player, GameState *currentState, bool *running);
void handleEndInput(SDL_Event *event, GameState *currentState, bool *running);

#endif // INPUT_H
