#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

// --- Constantes do Jogo ---
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SQUARE_SIZE 30
#define PLAYER_SPEED 5
#define ENEMY_SPEED 2
#define BULLET_SPEED 10
#define BULLET_SIZE 8

#define MAX_ENEMIES 20
#define MAX_BULLETS 50
#define ENEMY_SPAWN_INTERVAL 2000 // em milissegundos

// --- Estruturas de Dados ---

// Representa o jogador
typedef struct {
    SDL_Rect rect;
    int dx, dy; // Velocidade/direção atual
    int last_move_dx, last_move_dy; // Última direção de movimento (para atirar)
} Player;

// Representa um inimigo
typedef struct {
    SDL_Rect rect;
    bool active;
} Enemy;

// Representa um projétil
typedef struct {
    SDL_Rect rect;
    int dx, dy; // Direção do projétil
    bool active;
} Bullet;

// --- Funções Auxiliares ---

/**
 * Inicializa o jogador no centro da tela.
 */
void initPlayer(Player *player) {
    player->rect.w = SQUARE_SIZE;
    player->rect.h = SQUARE_SIZE;
    player->rect.x = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
    player->rect.y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;
    player->dx = 0;
    player->dy = 0;
    player->last_move_dx = 1; // Padrão: atirar para a direita
    player->last_move_dy = 0;
}

/**
 * Inicializa o array de inimigos (todos inativos).
 */
void initEnemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
    }
}

/**
 * Inicializa o array de projéteis (todos inativos).
 */
void initBullets(Bullet bullets[]) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}

/**
 * Tenta criar um novo projétil.
 */
void shootBullet(Player *player, Bullet bullets[]) {
    // Encontra um slot de projétil inativo
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].rect.w = BULLET_SIZE;
            bullets[i].rect.h = BULLET_SIZE;
            
            // Centraliza o projétil no jogador
            bullets[i].rect.x = player->rect.x + (player->rect.w / 2) - (bullets[i].rect.w / 2);
            bullets[i].rect.y = player->rect.y + (player->rect.h / 2) - (bullets[i].rect.h / 2);

            // Define a direção do projétil
            bullets[i].dx = player->last_move_dx * BULLET_SPEED;
            bullets[i].dy = player->last_move_dy * BULLET_SPEED;
            
            break; // Dispara apenas um por vez
        }
    }
}

/**
 * Tenta criar um novo inimigo em uma das bordas.
 */
void spawnEnemy(Enemy enemies[]) {
    // Encontra um slot de inimigo inativo
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].rect.w = SQUARE_SIZE;
            enemies[i].rect.h = SQUARE_SIZE;

            // Escolhe uma borda aleatória (0=topo, 1=baixo, 2=esq, 3=dir)
            int edge = rand() % 4;
            switch (edge) {
                case 0: // Topo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - SQUARE_SIZE);
                    enemies[i].rect.y = 0 - SQUARE_SIZE;
                    break;
                case 1: // Baixo
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - SQUARE_SIZE);
                    enemies[i].rect.y = SCREEN_HEIGHT;
                    break;
                case 2: // Esquerda
                    enemies[i].rect.x = 0 - SQUARE_SIZE;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - SQUARE_SIZE);
                    break;
                case 3: // Direita
                    enemies[i].rect.x = SCREEN_WIDTH;
                    enemies[i].rect.y = rand() % (SCREEN_HEIGHT - SQUARE_SIZE);
                    break;
            }
            break; // Cria apenas um por vez
        }
    }
}

/**
 * Processa a entrada do usuário (teclado).
 */
void handleInput(Player *player, Bullet bullets[], bool *running) {
    SDL_Event event;
    // Processa eventos na fila (ex: fechar janela, apertar tecla)
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
        // Evento de TECLA PRESSIONADA (apenas para ações de "tiro")
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    shootBullet(player, bullets);
                    break;
                case SDLK_ESCAPE:
                    *running = false;
                    break;
            }
        }
    }

    // Processa estado do teclado (para movimento contínuo)
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    
    player->dx = 0;
    player->dy = 0;

    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        player->dx = -PLAYER_SPEED;
        player->last_move_dx = -1;
        player->last_move_dy = 0;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        player->dx = PLAYER_SPEED;
        player->last_move_dx = 1;
        player->last_move_dy = 0;
    }
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
        player->dy = -PLAYER_SPEED;
        player->last_move_dx = 0;
        player->last_move_dy = -1;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
        player->dy = PLAYER_SPEED;
        player->last_move_dx = 0;
        player->last_move_dy = 1;
    }
}

/**
 * Atualiza o estado de todos os objetos do jogo.
 */
void update(Player *player, Enemy enemies[], Bullet bullets[], bool *running, Uint32 *lastEnemySpawnTime) {
    // 1. Atualizar Jogador
    player->rect.x += player->dx;
    player->rect.y += player->dy;

    // Manter jogador dentro da tela
    if (player->rect.x < 0) player->rect.x = 0;
    if (player->rect.y < 0) player->rect.y = 0;
    if (player->rect.x > SCREEN_WIDTH - player->rect.w) player->rect.x = SCREEN_WIDTH - player->rect.w;
    if (player->rect.y > SCREEN_HEIGHT - player->rect.h) player->rect.y = SCREEN_HEIGHT - player->rect.h;

    // 2. Atualizar Projéteis
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].rect.x += bullets[i].dx;
            bullets[i].rect.y += bullets[i].dy;

            // Desativar projéteis fora da tela
            if (bullets[i].rect.x < 0 || bullets[i].rect.x > SCREEN_WIDTH ||
                bullets[i].rect.y < 0 || bullets[i].rect.y > SCREEN_HEIGHT) {
                bullets[i].active = false;
            }
        }
    }

    // 3. Spawns de Inimigos
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - *lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL) {
        spawnEnemy(enemies);
        *lastEnemySpawnTime = currentTime;
    }

    // 4. Atualizar Inimigos (Perseguição)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Lógica simples de perseguição
            if (enemies[i].rect.x < player->rect.x) enemies[i].rect.x += ENEMY_SPEED;
            if (enemies[i].rect.x > player->rect.x) enemies[i].rect.x -= ENEMY_SPEED;
            if (enemies[i].rect.y < player->rect.y) enemies[i].rect.y += ENEMY_SPEED;
            if (enemies[i].rect.y > player->rect.y) enemies[i].rect.y -= ENEMY_SPEED;
        }
    }

    // 5. Detecção de Colisão
    
    // Projétil vs Inimigo
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) continue;

            if (SDL_HasIntersection(&bullets[i].rect, &enemies[j].rect)) {
                bullets[i].active = false; // Destrói projétil
                enemies[j].active = false; // Destrói inimigo
            }
        }
    }

    // Jogador vs Inimigo (Game Over)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                printf("GAME OVER! Você colidiu com um inimigo.\n");
                *running = false; // Encerra o jogo
            }
        }
    }
}

/**
 * Desenha todos os objetos na tela.
 */
void render(SDL_Renderer *renderer, Player player, Enemy enemies[], Bullet bullets[]) {
    // Limpa a tela (fundo preto)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Desenha o Jogador (Roxo)
    SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
    SDL_RenderFillRect(renderer, &player.rect);

    // Desenha os Inimigos (Vermelho)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            SDL_RenderFillRect(renderer, &enemies[i].rect);
        }
    }

    // Desenha os Projéteis (Branco)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_RenderFillRect(renderer, &bullets[i].rect);
        }
    }

    // Apresenta o que foi desenhado na tela
    SDL_RenderPresent(renderer);
}

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
    SDL_Window *window = SDL_CreateWindow("Caça aos Quadrados",
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

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // --- Estado do Jogo ---
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    bool running = true;
    Uint32 lastEnemySpawnTime = 0;

    // Inicializa os objetos
    initPlayer(&player);
    initEnemies(enemies);
    initBullets(bullets);

    // --- Loop Principal do Jogo ---
    while (running) {
        
        // 1. Processar Entradas
        handleInput(&player, bullets, &running);
        
        // 2. Atualizar Estado do Jogo
        update(&player, enemies, bullets, &running, &lastEnemySpawnTime);
        
        // 3. Renderizar (Desenhar) na Tela
        render(renderer, player, enemies, bullets);

        // Limita o framerate para ~60 FPS
        SDL_Delay(16); 
    }

    // --- Limpeza ---
    cleanup(window, renderer);
    return 0;
}
