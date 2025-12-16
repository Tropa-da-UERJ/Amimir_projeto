#include "init.h"
#include <stdlib.h> // Para rand()
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

#define PLAYER_SCALE_FACTOR 3

static SDL_Texture* carregarTextura(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* novaTextura = IMG_LoadTexture(renderer, path);
    if (novaTextura == NULL) {
        printf("Não foi possível carregar a textura '%s'! IMG_Error: %s\n", path, IMG_GetError());
    }
    return novaTextura;
}

SDL_Texture* createSpotlightTexture(SDL_Renderer* renderer, int radius) {
    int diameter = radius * 2;
    // Cria uma superfície temporária
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, diameter, diameter, 32, SDL_PIXELFORMAT_RGBA8888);
    
    if (!surface) return NULL;

    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    
    // Percorre cada pixel para desenhar um círculo com gradiente
    for (int y = 0; y < diameter; y++) {
        for (int x = 0; x < diameter; x++) {
            int cx = x - radius;
            int cy = y - radius;
            float distance = sqrtf(cx * cx + cy * cy);

            if (distance < radius) {
                // Luz: Centro Branco (255,255,255) -> Borda Preta (0,0,0)
                float factor = 1.0f - (distance / radius);
                factor = factor * factor; // Suavização quadrática (fica mais bonito)
                
                Uint8 intensity = (Uint8)(255 * factor);
                
                // IMPORTANTE: A Alpha aqui ajuda a suavizar a borda
                pixels[y * diameter + x] = SDL_MapRGBA(surface->format, intensity, intensity, intensity, 255);
            } else {
                pixels[y * diameter + x] = SDL_MapRGBA(surface->format, 0, 0, 0, 255);
            }
        }
    }
    SDL_UnlockSurface(surface);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    // A luz será SOMADA ao ambiente escuro
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
    
    return texture;
}

//--- Inicializa o sistema de iluminação ---
void initLighting(App *app, SDL_Renderer *renderer) {
    // 1. Cria a camada de ambiente (tamanho da tela)
    app->texDarknessLayer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // MOD: A escuridão vai multiplicar as cores do jogo.
    // Branco = Jogo normal. Cinza = Jogo escuro.   
    SDL_SetTextureBlendMode(app->texDarknessLayer, SDL_BLENDMODE_MOD);

    // 2. Cria a textura do holofote (raio de 200px)
    app->texSpotlight = createSpotlightTexture(renderer, 200); 
}


void initPlayer(Player *player, SDL_Renderer *renderer) {
    player->texture = carregarTextura("assets/player_sprite.png", renderer);
    
    int frameOriginalWidth = 32;
    int frameOriginalHeight = 32;

    // aplicação da ampliação do sprite:
    player->rect.w = frameOriginalWidth * PLAYER_SCALE_FACTOR;  // 32 * 3 = 96
    player->rect.h = frameOriginalHeight * PLAYER_SCALE_FACTOR; // 32 * 3 = 96
    
    player->rect.x = (SCREEN_WIDTH - player->rect.w) / 2;
    player->rect.y = (SCREEN_HEIGHT - player->rect.h) / 2;
    player->dx = 0;
    player->dy = 0;

    // stats básicos
    player->max_hp = PLAYER_STARTING_HP;
    player->hp = player->max_hp;
    player->invulnerable = false;
    player->invulnerableTime = 0;
    player->points = 0;
    player->xp = 0;
    player->xp_toLevel = PLAYER_XP_TOLEVEL;
    player->speed = PLAYER_SPEED;
    player->fire_delay = PLAYER_FIRE_DELAY;

    // stats avançados
    player->damage = 1.0f;   
    player->multishot_level = 0; 
    player->bullet_speed_mult = 1.0f;
    player->dodge_chance = 0.0f;

    player->frameX = 0; // Inicia na posição 0
}

void initEnemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        // garante que comecem "mortos"
        enemies[i].active = false; 
    }
}

void initBullets(Bullet bullets[]) {
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}

// Renomeado de eBullets para clareza
void initEnemyBullets(Bullet enemyBullets[]) {
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
        enemyBullets[i].active = false;
    }
}

void initRound(App *app) {
    app->roundStartTime = SDL_GetTicks();
    app->elapsedTime = 0;
    app->currentDarknessAlpha = 0; // Começa claro

}
