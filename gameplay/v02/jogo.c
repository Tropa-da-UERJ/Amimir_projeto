#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <math.h> //compilar com -lm

// --- Constantes do Jogo ---
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define SQUARE_SIZE 30
#define PLAYER_SPEED 5
#define ENEMY_SPEED 2
#define BULLET_SPEED 10
#define BULLET_SIZE 8
#define FIRE_DELAY 500  // ms entre tiros (ajuste para mais/menos frequência)
#define INVULNERABILITY_DURATION 1000  // 1000 ms = 1 segundo de invulnerabilidade

#define MAX_ENEMIES 5 // ou 40 (mais inimigos na tela ao mesmo tempo)
#define MAX_BULLETS 50
#define ENEMY_SPAWN_INTERVAL 250 // em milissegundos ou 800 (mais rapido)
#define PLAYER_STARTING_LIVES 1000
#define ANXIETY_STARTING_LIVES 5

#define SEPARATION_STRENGTH 0.3f  // quanto eles se afastam (0.1–0.5 é bom)
#define MAX_OVERLAP 15           // permite até 4px de interseção antes de reagir

// --- Estruturas de Dados ---

// Representa o jogador
typedef struct {
    SDL_Rect rect;
    int dx, dy; // Velocidade/direção atual
    int lives;
    Uint32 lastShotTime; // <<--- novo: timestamp do último tiro (SDL_GetTicks)
    bool invulnerable;      // <- novo: está invulnerável?
    Uint32 invulnerableTime;// <- novo: momento do último hit (SDL_GetTicks)
} Player;

// Representa um inimigo
typedef struct {
    SDL_Rect rect;
    bool active;
    int lives;
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
    player->lives = PLAYER_STARTING_LIVES;
    player->invulnerable = false;
    player->invulnerableTime = 0;

}

/**
 * Inicializa o array de inimigos (todos inativos).
 */
void initEnemies(Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].lives = ANXIETY_STARTING_LIVES;
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

void separateEnemies(Enemy enemies[], int enemyCount) {
    SDL_Rect overlap;

    for (int i = 0; i < enemyCount; i++) {
        for (int j = i + 1; j < enemyCount; j++) {

            if (SDL_HasIntersection(&enemies[i].rect, &enemies[j].rect)) {
                SDL_IntersectRect(&enemies[i].rect, &enemies[j].rect, &overlap);

                // Só separa se a interseção for maior que o limite permitido
                if (overlap.w > MAX_OVERLAP || overlap.h > MAX_OVERLAP) {

                    // Decide eixo principal da separação
                    if (overlap.w < overlap.h) {
                        // Mais estreito no eixo X → separa horizontalmente
                        if (enemies[i].rect.x < enemies[j].rect.x) {
                            enemies[i].rect.x -= (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.x += (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.x += (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.x -= (int)(overlap.w * 0.5f * SEPARATION_STRENGTH);
                        }
                    } else {
                        // Mais estreito no eixo Y → separa verticalmente
                        if (enemies[i].rect.y < enemies[j].rect.y) {
                            enemies[i].rect.y -= (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.y += (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                        } else {
                            enemies[i].rect.y += (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                            enemies[j].rect.y -= (int)(overlap.h * 0.5f * SEPARATION_STRENGTH);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Tenta criar um novo projétil.
 */
void shootBullet(Player *player, Bullet bullets[]) {
    // procura um projétil livre
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].rect.w = BULLET_SIZE;
            bullets[i].rect.h = BULLET_SIZE;

            // posiciona no centro do jogador
            bullets[i].rect.x = player->rect.x + (player->rect.w / 2) - (bullets[i].rect.w / 2);
            bullets[i].rect.y = player->rect.y + (player->rect.h / 2) - (bullets[i].rect.h / 2);

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // direção do jogador -> mouse
            float dirX = (float)mouseX - (bullets[i].rect.x + bullets[i].rect.w / 2.0f);
            float dirY = (float)mouseY - (bullets[i].rect.y + bullets[i].rect.h / 2.0f);

            // normaliza
            float len = sqrtf(dirX * dirX + dirY * dirY);
            if (len == 0) {
                // evita divisão por zero: atira para a direita por padrão
                bullets[i].dx = BULLET_SPEED;
                bullets[i].dy = 0;
            } else {
                bullets[i].dx = (int)roundf((dirX / len) * BULLET_SPEED);
                bullets[i].dy = (int)roundf((dirY / len) * BULLET_SPEED);
            }

            break; // atira apenas um projétil por chamada
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

    // --- LÓGICA DE MOVIMENTO MODIFICADA ---
    // Usamos -= e += para que teclas opostas (esquerda/direita) 
    // pressionadas ao mesmo tempo se anulem.
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        player->dx -= PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        player->dx += PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
        player->dy -= PLAYER_SPEED;
    }
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
        player->dy += PLAYER_SPEED;
    }

}
/**
 * Atualiza o estado de todos os objetos do jogo.
 */
void update(Player *player, Enemy enemies[], Bullet bullets[], bool *running, Uint32 *lastEnemySpawnTime) {
    // 1. Atualizar Jogador
    player->rect.x += player->dx;
    player->rect.y += player->dy;

    Uint32 now = SDL_GetTicks();
        if (now - player->lastShotTime >= FIRE_DELAY) {
        shootBullet(player, bullets);
        player->lastShotTime = now;
        }

    // Manter jogador dentro da tela
    if (player->rect.x < 0) player->rect.x = 0;
    if (player->rect.y < 0) player->rect.y = 0;
    if (player->rect.x > SCREEN_WIDTH - player->rect.w) player->rect.x = SCREEN_WIDTH - player->rect.w;
    if (player->rect.y > SCREEN_HEIGHT - player->rect.h) player->rect.y = SCREEN_HEIGHT - player->rect.h;

    // Invulnerabilidade do player
    if (player->invulnerable) {
    Uint32 now = SDL_GetTicks();
        if (now - player->invulnerableTime >= INVULNERABILITY_DURATION) {
        player->invulnerable = false;
        }
    }

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
        separateEnemies(enemies, MAX_ENEMIES);
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
                enemies[i].lives--;
                if (enemies[i].lives <= 0) {
                    enemies[i].active = false; // Destrói inimigo
                }
            }
        }
    }

    // Jogador vs Inimigo (Game Over)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (!player->invulnerable && SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                
                player->lives--; // reduz a vida
                printf("Vida perdida! Vidas restantes: %d\n", player->lives);
                player->invulnerable = true;
                player->invulnerableTime = SDL_GetTicks();

                enemies[i].lives--;

                if (player->lives <= 0) {
                    // Se não há mais vidas, fim de jogo
                    printf("GAME OVER! Você não tem mais vidas.\n");
                    *running = false; // Encerra o jogo
                } 
                if (enemies[i].lives <= 0) {
                    enemies[i].active = false; // Destrói inimigo
                }
                                
                break; 
            }
            else if (player->invulnerable && SDL_HasIntersection(&player->rect, &enemies[i].rect)) {
                
                enemies[i].lives--;

                if (enemies[i].lives <= 0) {
                    enemies[i].active = false; // Destrói inimigo
               }

                break; 
            }
        }
    }
}

/**
 * Desenha todos os objetos na tela.
 */
void render(SDL_Renderer *renderer, Player player, Enemy enemies[], Bullet bullets[]) {
    // Limpa a tela (fundo azul)
    SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255);
    SDL_RenderClear(renderer);

    // Desenha o Jogador (Roxo)
    //SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
    //SDL_RenderFillRect(renderer, &player.rect);
    if (player.invulnerable) {
        Uint32 now = SDL_GetTicks();
        // pisca a cada 100ms
        if ((now / 100) % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // normal
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255); // fundo (efeito "invisível")
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // roxo normal
    }
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
    SDL_Window *window = SDL_CreateWindow("A mimir v0.2",
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

        // 3. Atualizar Título da Janela (Feedback de Vidas)
        if (running) { // Só atualiza se o jogo não acabou de terminar
            char windowTitle[100];
            // sprintf é usado para formatar uma string
            sprintf(windowTitle, "A mimir v0.2 - Vida: %d", player.lives);
            SDL_SetWindowTitle(window, windowTitle);
        }
        
        // 4. Renderizar (Desenhar) na Tela
        render(renderer, player, enemies, bullets);

        // Limita o framerate para ~60 FPS
        SDL_Delay(16); 
    }

    // --- Limpeza ---
    cleanup(window, renderer);
    return 0;
}
