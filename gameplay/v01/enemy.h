#pragma once

#include <SDL.h>
#include "player.h" // Pode ser necessário para colisões ou IA

// --- Tipos de Inimigos ---
typedef enum {
    ENEMY_TYPE_SLIME,
    ENEMY_TYPE_GHOST,
    ENEMY_TYPE_ORC,
    ENEMY_TYPE_BOSS // Seu quarto inimigo
} EnemyType;

// --- Estrutura de um Único Inimigo ---
typedef struct {
    SDL_Rect rect;
    float x_pos;
    float y_pos;
    float x_vel;
    float y_vel;
    int health;
    EnemyType type;
    SDL_Texture* texture;
    // Adicione variáveis específicas do inimigo (ex: tempo de ataque)
} Enemy;

// --- Funções de Gerenciamento ---

// Cria um inimigo de um tipo específico
Enemy createEnemy(SDL_Renderer* renderer, EnemyType type, int startX, int startY);

// Carrega todas as texturas de inimigos (chamado uma vez na inicialização do jogo)
void loadEnemyTextures(SDL_Renderer* renderer);

// Limpa todas as texturas de inimigos (chamado no encerramento)
void cleanupEnemyTextures();

// Atualiza a lógica de um inimigo (movimento, IA, etc.)
void updateEnemy(Enemy* enemy, const Player* player); // IA pode precisar da posição do jogador

// Renderiza um inimigo
void renderEnemy(SDL_Renderer* renderer, const Enemy* enemy);

// Lógica de colisão
int checkCollision(const SDL_Rect* rectA, const SDL_Rect* rectB);
