#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Nossos módulos customizados
#include "defs.h"
#include "init.h"
#include "input.h"
#include "update.h"
#include "render.h"
#include "text.h"

/**
 * Libera os recursos da SDL.
 */
void cleanup(SDL_Window *window, SDL_Renderer *renderer, App *app) {
    if (app->font) {
        TTF_CloseFont(app->font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// --- Função Principal ---
int main(void) {
    // Inicialização da SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", TTF_GetError());
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
    
    GameState currentState = STATE_PLAYING;

    App app = { .font = NULL };

    // Carregar a fonte
    // font.ttf é o arquivo fonte, 24 é o tamanho
    app.font = TTF_OpenFont("ComicReliefRegular.ttf", 24);
    if (!app.font) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", TTF_GetError());
    }

    // Inicializa os objetos (usando nosso módulo init)
    initPlayer(&player);
    initEnemies(enemies);
    initBullets(bullets);
    initEnemyBullets(enemyBullets); // Nome atualizado

    // --- Loop Principal do Jogo ---
    while (running) {
        
        // 1. Processar Entradas
        if (currentState == STATE_PLAYING) {
            handleInput(&player, &running);
        } else if (currentState == STATE_LEVELUP) {
            handleLevelUpInput(&player, &currentState, &running);
        }
        
        // 2. Atualizar Estado do Jogo
        if (currentState == STATE_PLAYING) {
            update(&player, enemies, bullets, enemyBullets, &running, &lastEnemySpawnTime);
        }
        
        // 3. Verificar LevelUP
        if (currentState == STATE_PLAYING && player.xp >= player.xp_toLevel) {

            currentState =  STATE_LEVELUP;
            
            player.xp -= player.xp_toLevel; //consome o xp
            player.xp_toLevel = (int)(player.xp_toLevel * PLAYER_XP_LEVEL_SCALING); //aumenta o custo do proximo nivel

            // mostra as opções de level up NO CONSOLE
            printf("\n -- LEVEL UP --\n");
            printf("Escolha seu upgrade:\n");
            printf("1. +5 HP Máximo\n");
            printf("2. +10%% Cadência de Tiro\n");
            printf("3. +1 Velocidade de Movimento\n");
            printf("Pressione 1, 2 ou 3...\n");
        } 
            
        // 4. Atualizar Título da Janela
        if (running) {
            char windowTitle[100];
            sprintf(windowTitle, "A mimir v0.5 - HP: %d/%d | Pontos: %d | XP: %d/%d", player.hp, player.max_hp, player.points, player.xp, player.xp_toLevel);
            SDL_SetWindowTitle(window, windowTitle);
        }
        
        // 5. Renderizar (módulo render)
        render(renderer, &app, player, enemies, bullets, enemyBullets, currentState);

        SDL_Delay(16); // Limita para ~60 FPS
    }

    // --- Limpeza ---
    cleanup(window, renderer, &app);
    return 0;
}
