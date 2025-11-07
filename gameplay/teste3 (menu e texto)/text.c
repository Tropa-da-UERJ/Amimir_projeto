#include "text.h"
#include "stdio.h"

void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {

    if (!font) {
        fprintf(stderr, "Erro: Font TTF não carregada.\n");
        return;
    }
    
    // 1. Cria a superfície do texto
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);

    if (!surface) {
        fprintf(stderr, "Erro ao renderizar texto: %s. Texto: %s\n", TTF_GetError(), text); // <-- DEBUG
        return;
    }
    
    // 2. Converte superficie em textura
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // 3. Limpa a superficie
    SDL_FreeSurface(surface);

    if (!texture) {
        fprintf(stderr, "Erro ao criar textura do texto: %s. Texto: %s\n", SDL_GetError(), text); // <-- DEBUG
        return;
    }

    // 4. Define onde a textura será desenhada
    SDL_Rect dest = {x, y, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    // 5. Desenha a textura na tela
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    // 6. Limpa a textura
    SDL_DestroyTexture(texture);

} 
