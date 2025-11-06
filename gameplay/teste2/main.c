#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

// Nossos módulos customizados
#include "defs.h"
#include "init.h"
#include "input.h"
#include "update.h"
#include "render.h"

/**
 * Libera os recursos da SDL.
 */
void cleanup(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// --- Função Principal ---
int main(int argc, char *argv[]) {
    // Inicialização da SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Criação da Janela
    SDL_Window *window = SDL_CreateWindow("A mimir v0.5",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criação do Renderizador
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Erro ao criar renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));

    // --- Estado do Jogo ---
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[PLAYER_MAX_BULLETS];
    Bullet enemyBullets[CAFEINA_MAX_BULLETS];
    bool running = true;
    Uint32 lastEnemySpawnTime = 0;

    // Inicializa os objetos (usando nosso módulo init)
    initPlayer(&player);
    initEnemies(enemies);
    initBullets(bullets);
    initEnemyBullets(enemyBullets); // Nome atualizado

    // --- Loop Principal do Jogo ---
    while (running) {
        
        // 1. Processar Entradas (módulo input)
        handleInput(&player, &running);
        
        // 2. Atualizar Estado do Jogo (módulo update)
        update(&player, enemies, bullets, enemyBullets, &running, &lastEnemySpawnTime);

        // 3. Atualizar Título da Janela
        if (running) {
            char windowTitle[100];
            sprintf(windowTitle, "A mimir v0.5 - Vida: %d | Pontos: %d | XP: %d", player.hp, player.points, player.xp);
            SDL_SetWindowTitle(window, windowTitle);
        }
        
        // 4. Renderizar (módulo render)
        render(renderer, player, enemies, bullets, enemyBullets);

        SDL_Delay(16); // Limita para ~60 FPS
    }

    // --- Limpeza ---
    cleanup(window, renderer);
    return 0;
}
