// Em game.c
#include "game.h"
#include "player.h"
#include "enemy.h"

#define MAX_ENEMIES 10

// Variáveis globais (privadas) do módulo game.c
static Player gPlayer;
static Enemy gEnemies[MAX_ENEMIES];
static int gNumEnemies = 0;

void initGame(SDL_Renderer* renderer) {
    // 1. Inicializa o Jogador
    gPlayer = createPlayer(renderer, "assets/player.png", 100, 100);
    
    // 2. Inicializa as texturas dos inimigos
    loadEnemyTextures(renderer);
    
    // 3. Cria os 4 inimigos iniciais
    gEnemies[0] = createEnemy(renderer, ENEMY_TYPE_SLIME, 500, 200);
    gEnemies[1] = createEnemy(renderer, ENEMY_TYPE_GHOST, 600, 400);
    gEnemies[2] = createEnemy(renderer, ENEMY_TYPE_ORC, 300, 500);
    gEnemies[3] = createEnemy(renderer, ENEMY_TYPE_BOSS, 700, 100);
    gNumEnemies = 4;
}

void updateGame() {
    // 1. Atualiza o jogador
    updatePlayer(&gPlayer);
    
    // 2. Atualiza todos os inimigos
    for (int i = 0; i < gNumEnemies; i++) {
        updateEnemy(&gEnemies[i], &gPlayer);
        
        // 3. Verifica colisão entre Jogador e Inimigo
        if (gPlayer.health > 0 && checkCollision(&gPlayer.rect, &gEnemies[i].rect)) {
            // Se colidiu, o jogador toma dano (simplificado)
            gPlayer.health -= 5;
            // (Você faria um knockback ou animação de dano aqui)
        }
    }
    // ... Lógica de fim de jogo, etc. ...
}

void renderGame(SDL_Renderer* renderer) {
    // 1. Renderiza o jogador
    renderPlayer(renderer, &gPlayer);
    
    // 2. Renderiza os inimigos
    for (int i = 0; i < gNumEnemies; i++) {
        renderEnemy(renderer, &gEnemies[i]);
    }
}

// ... handleGameInput e cleanupGame ...
