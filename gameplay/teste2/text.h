#ifndef TEXT_H
#define TEXT_H

#include "defs.h"

//função para desenhar texto
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);

#endif // TEXT_H
