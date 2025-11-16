#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Nossos módulos customizados
#include "defs.h"
#include "init.h"
#include "input.h"
#include "update.h"
#include "render.h"
#include "text.h"
#include "menu.h"

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

    if (!(IMG_Init(IMG_INIT_PNG))) {
    printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
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
    Enemy enemies[MAX_ENEMIES] = {0};
    Bullet bullets[PLAYER_MAX_BULLETS] = {0};
    Bullet enemyBullets[CAFEINA_MAX_BULLETS] = {0};
    Uint32 lastEnemySpawnTime = 0;
    
    GameState currentState = STATE_MENU; // Começa no Menu
    GameState previousState = STATE_MENU; // Variável para rastrear reinício

    menu_init(renderer);
    bool running = true;
    SDL_Event event;

    App app = { .font = NULL };

    // Carregar a fonte
    // font.ttf é o arquivo fonte, 24 é o tamanho "padrão"
    app.font = TTF_OpenFont("ComicReliefRegular.ttf", 24);
    if (!app.font) {
        fprintf(stderr, "Erro ao carregar a fonte: %s\n", TTF_GetError());
    }

    // Inicializa os objetos (usando nosso módulo init)
    initPlayer(&player, renderer);
    initEnemies(enemies);
    initBullets(bullets);
    initEnemyBullets(enemyBullets); // Nome atualizado

    // --- Loop Principal do Jogo ---
    while (running) {

        previousState = currentState; // Atualiza o estado anterior

        // --- PROCESSAMENTO DE ENTRADAS (INPUT) ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                currentState = STATE_QUIT;
            }

            // Direciona a entrada para o módulo correto
            switch (currentState) {
                case STATE_MENU:
                    menu_handle_input(&event, &currentState);
                    break;
                case STATE_PLAYING:
                    handlePlayingInput_Events(&event, &running);
                    break;
                case STATE_LEVELUP:
                    handleLevelUpInput(&event, &player, &currentState, &running);
                    break;
                case STATE_END:
                    handleEndInput(&event, &currentState, &running);
                    break;
                // case GAME_STATE_SCOREBOARD:
                //    handleScoreboardInput(&currentState, &running); // Se existir
                //    break;
                default:
                    break;
            }
        }

        // Se acabamos de sair do menu e entrar no jogo...
        if (currentState == STATE_PLAYING && (previousState == STATE_MENU || previousState == STATE_END)) {
            // ... inicializa/reseta o jogo
            initPlayer(&player);
            initEnemies(enemies);
            initBullets(bullets);
            initEnemyBullets(enemyBullets);
            lastEnemySpawnTime = SDL_GetTicks();
        }
        
        // Se saímos do Fim de Jogo/Placar e voltamos ao menu...
        //if ((previousState == STATE_END || previousState == STATE_SCOREBOARD) && currentState == STATE_MENU) {
        //    initMenu(); 
        //}

        // 3. Atualizar Estado do Jogo
        if (currentState == STATE_PLAYING) {

            handlePlayingInput_State(&player);
            
            update(&player, enemies, bullets, enemyBullets, &running, &lastEnemySpawnTime, &currentState);
        }
                        
        // 4. Verificar LevelUP
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
            
        // 5. Atualizar Título da Janela
        if (running) {
            char windowTitle[100];
            if (currentState == STATE_PLAYING || currentState == STATE_LEVELUP) {
                sprintf(windowTitle, "A mimir v0.6 - HP: %d/%d | Pontos: %d", player.hp, player.max_hp, player.points);
            } else if (currentState == STATE_MENU) {
                sprintf(windowTitle, "A mimir v0.6 - Menu Principal");
            } else if (currentState == STATE_END) {
                sprintf(windowTitle, "A mimir v0.6 - Fim de Jogo!");
            }
            SDL_SetWindowTitle(window, windowTitle);
        }
        
        // 5. Renderizar (módulo render)
        render(renderer, &app, player, enemies, bullets, enemyBullets, currentState);

        // Verificação de 'running' (o estado QUIT cuida disso)
        if (currentState == STATE_QUIT) {
            running = false;
        }

        SDL_Delay(16); // Limita para ~60 FPS
    }

    // --- Limpeza ---
    cleanup(window, renderer, &app);
    
    menu_destroy();

    IMG_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    return 0;
}
