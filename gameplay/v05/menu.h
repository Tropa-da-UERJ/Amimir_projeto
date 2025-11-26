#ifndef MENU_H
#define MENU_H
#include "defs.h"

void menu_init(SDL_Renderer *renderer);
void menu_handle_input(SDL_Event *event, GameState *currentState);
void menu_render(SDL_Renderer *renderer);
void menu_destroy(void);
bool menu_is_in_splash(void);

#endif // MENU_H
