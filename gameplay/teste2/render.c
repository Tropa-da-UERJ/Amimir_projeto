#include "render.h"

void render(SDL_Renderer *renderer, Player player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], GameState currentState) {
    // Limpa a tela (fundo azul)
    SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255);
    SDL_RenderClear(renderer);

    // Desenha o Jogador
    if (player.invulnerable) {
        Uint32 now = SDL_GetTicks();
        if ((now / 100) % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // roxo
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255); // cor do fundo
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // roxo normal
    }
    SDL_RenderFillRect(renderer, &player.rect);

    // Desenha os Inimigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (enemies[i].type == 0) {
              SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // ANSIEDADE
            } else {
              SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // CAFEINA
            }
            SDL_RenderFillRect(renderer, &enemies[i].rect);
        }
    }

    // Desenha os Projéteis (Branco)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_RenderFillRect(renderer, &bullets[i].rect);
        }
    }
    
    // Desenha os Projéteis Inimigos (pretos)
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        if (enemyBullets[i].active) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // preto
            SDL_RenderFillRect(renderer, &enemyBullets[i].rect);
        }
    }
    
    if (currentState == STATE_LEVELUP) {
        
        SDL_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND); // blend permite transparencia

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // cor preta semi-transparente

        SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &screenRect);
        
    }

    SDL_RenderPresent(renderer);
}
