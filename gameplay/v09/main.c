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
#include "fim.h"

/**
 * Libera os recursos da SDL.
 */
void cleanup(SDL_Window *window, SDL_Renderer *renderer, App *app, Player *player) {
    if (app->font) TTF_CloseFont(app->font);
    if (app->texBullet) SDL_DestroyTexture(app->texBullet);
    if (app->texAnsiedade) SDL_DestroyTexture(app->texAnsiedade);
    if (app->texCafeina) SDL_DestroyTexture(app->texCafeina);
    if (player->texture) SDL_DestroyTexture(player->texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// --- Função Principal ---
int main(void) {
    // Inicialização da SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG))) return 1;
    if (TTF_Init() == -1) return 1;

    // Criação da Janela
    SDL_Window *window = SDL_CreateWindow("A mimir v0.8",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    // Criação do Renderizador
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
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
    fim_init(renderer);
    bool running = true;
    SDL_Event event;

    App app = { .font = NULL };

    // Carregar a fonte
    // font.ttf é o arquivo fonte, 24 é o tamanho "padrão"
    app.font = TTF_OpenFont("ComicReliefRegular.ttf", 24);

    SDL_Surface* surfBullet = TTF_RenderText_Solid(app.font, "Z", (SDL_Color){255, 255, 255, 255});
    if (surfBullet) {
        app.texBullet = SDL_CreateTextureFromSurface(renderer, surfBullet);
        SDL_FreeSurface(surfBullet); // Libera a surface temporária
    } else {
        printf("Erro ao criar textura da bala Z: %s\n", TTF_GetError());
        app.texBullet = NULL;
    }

    app.texCafeina = IMG_LoadTexture(renderer, "assets/cafeina.png");
    app.texAnsiedade = IMG_LoadTexture(renderer, "assets/ansiedade.png");

    if (!app.texAnsiedade) {
        printf("Erro ao carregar textura da Ansiedade: %s\n", IMG_GetError());
    }

    if (!app.texCafeina) {
        printf("Erro ao carregar textura da Cafeína: %s\n", IMG_GetError());
    }

    // Inicializa os objetos (usando nosso módulo init)
    initPlayer(&player, renderer);
    initEnemies(enemies);
    initLighting(&app, renderer);
    initBullets(bullets);
    initEnemyBullets(enemyBullets);
    initRound(&app);

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
                    handleLevelUpInput(&event, &player, &currentState, &running, &app);
                    break;
                case STATE_END:
                    fim_handle_input(&event, &currentState);
                    break;
                // case GAME_STATE_SCOREBOARD:
                //    handleScoreboardInput(&currentState, &running);
                //    break;
                default:
                    break;
            }
        }

        // Se acabamos de sair do menu e entrar no jogo...
        if (currentState == STATE_PLAYING && (previousState == STATE_MENU || previousState == STATE_END)) {
            // ... inicializa/reseta o jogo
            initPlayer(&player, renderer);
            initEnemies(enemies);
            initBullets(bullets);
            initEnemyBullets(enemyBullets);
            lastEnemySpawnTime = SDL_GetTicks();
            initRound(&app);
        }
        
        //Se saímos do Fim de Jogo/Placar e voltamos ao menu...
        if ((previousState == STATE_END || previousState == STATE_SCOREBOARD) && currentState == STATE_MENU) {
            menu_init(renderer);
        }

        // 3. Atualizar Estado do Jogo
        if (currentState == STATE_PLAYING) {

            handlePlayingInput_State(&player);
            
            update(&player, enemies, bullets, enemyBullets, &running, &lastEnemySpawnTime, &currentState, &app);
        }
                        
        // 4. Verificar LevelUP
        if (currentState == STATE_PLAYING && player.xp >= player.xp_toLevel) {

            currentState =  STATE_LEVELUP;
            
            player.xp -= player.xp_toLevel; //consome o xp
            player.xp_toLevel = (int)(player.xp_toLevel * PLAYER_XP_LEVEL_SCALING); //aumenta o custo do proximo nivel

            // sorteia 3 upgrades diferentes
            int picked[3] = {-1, -1, -1};
            for (int i = 0; i < 3; i++) {
                bool unique;
                int r;
                do {
                    unique = true;
                    r = rand() % UPGRADE_COUNT;
                    for (int j = 0; j < i; j++) {
                        if (picked[j] == r) unique = false;
                    }
                } while (!unique);
                picked[i] = r;
                app.currentUpgradeOptions[i] = (UpgradeType)r;
            }
            printf("Level Up! Opções geradas.\n");
        } 
            
        // 5. Atualizar Título da Janela
        if (running) {
            char windowTitle[100];
            if (currentState == STATE_PLAYING || currentState == STATE_LEVELUP) {
                sprintf(windowTitle, "A mimir v0.8 - HP: %d/%d | Pontos: %d", player.hp, player.max_hp, player.points);
            } else if (currentState == STATE_MENU) {
                sprintf(windowTitle, "A mimir v0.8 - Menu Principal");
            } else if (currentState == STATE_END) {
                sprintf(windowTitle, "A mimir v0.8 - Fim de Jogo!");
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
    cleanup(window, renderer, &app, &player);
    
    menu_destroy();

    fim_destroy();

    IMG_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    return 0;
}
