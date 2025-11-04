#include "enemy.h"
#include <SDL_image.h>
#include <stdio.h>

#define NUM_ENEMY_TYPES 4

// Variáveis estáticas (privadas a este arquivo) para as texturas dos 4 inimigos
static SDL_Texture* gEnemyTextures[NUM_ENEMY_TYPES] = {NULL};

// Implementação simples de colisão (AABB)
int checkCollision(const SDL_Rect* rectA, const SDL_Rect* rectB) {
    return (rectA->x < rectB->x + rectB->w &&
            rectA->x + rectA->w > rectB->x &&
            rectA->y < rectB->y + rectB->h &&
            rectA->y + rectA->h > rectB->y);
}

// ... implementação de loadEnemyTextures e cleanupEnemyTextures ...

Enemy createEnemy(SDL_Renderer* renderer, EnemyType type, int startX, int startY) {
    Enemy newEnemy;
    newEnemy.type = type;
    newEnemy.texture = gEnemyTextures[type]; // Pega a textura do array global
    
    // Lógica para definir atributos (diferentes para cada tipo!)
    if (type == ENEMY_TYPE_SLIME) {
        newEnemy.health = 10;
        newEnemy.x_vel = 1.0f;
    } else if (type == ENEMY_TYPE_GHOST) {
        newEnemy.health = 5; // Mais fraco
        newEnemy.x_vel = 0.0f; // Flutua
        newEnemy.y_vel = -0.5f;
    } 
    // ... defina os outros dois tipos ...

    // Define w/h baseado na textura (assumindo que já foi carregada)
    if (newEnemy.texture) {
        SDL_QueryTexture(newEnemy.texture, NULL, NULL, &newEnemy.rect.w, &newEnemy.rect.h);
    } else {
        newEnemy.rect.w = 32; // Default
        newEnemy.rect.h = 32;
    }
    
    // Inicialização de posição
    newEnemy.x_pos = (float)startX;
    newEnemy.y_pos = (float)startY;
    newEnemy.rect.x = startX;
    newEnemy.rect.y = startY;

    return newEnemy;
}

void updateEnemy(Enemy* enemy, const Player* player) {
    // Exemplo de IA simples: Focar no jogador
    if (enemy->type == ENEMY_TYPE_ORC) {
        if (player->x_pos < enemy->x_pos) {
            enemy->x_vel = -2.0f;
        } else {
            enemy->x_vel = 2.0f;
        }
        // ... Lógica para perseguir em Y, se aplicável ...
    }
    
    // Aplica movimento
    enemy->x_pos += enemy->x_vel;
    enemy->y_pos += enemy->y_vel;
    
    // Atualiza o SDL_Rect
    enemy->rect.x = (int)enemy->x_pos;
    enemy->rect.y = (int)enemy->y_pos;
    
    // (Aqui você adicionaria lógica de ataque, colisão, etc.)
}

void renderEnemy(SDL_Renderer* renderer, const Enemy* enemy) {
    if (enemy->texture) {
        SDL_RenderCopy(renderer, enemy->texture, NULL, &enemy->rect);
    }
}
