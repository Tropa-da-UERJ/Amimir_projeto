#ifndef FIM_H
#define FIM_H
#include "defs.h"

void fim_init(SDL_Renderer *renderer);
void fim_handle_input(SDL_Event *event, GameState *currentState);
void fim_render(SDL_Renderer *renderer, Player player, App *app);
void fim_destroy(void);

#endif // FIM_H
