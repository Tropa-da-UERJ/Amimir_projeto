#pragma once
#include <SDL.h>

typedef struct {
    SDL_Rect rect;          // Posição e tamanho (para renderização e colisão)
    SDL_Texture* texture;   // Imagem do personagem
    float x_pos;            // Posição real em float para movimento suave
    float y_pos;
    float x_vel;            // Velocidade (em pixels/frame)
    int health;             // Vida atual
    int score;              // Pontuação do jogador
    // Adicione um estado de animação, se necessário (ex: int state)
} Player;

Player createPlayer(SDL_Renderer* renderer, int x, int y);
void handlePlayerInput(Player* player, const Uint8* keyboardState);
void updatePlayer(Player* player);
void renderPlayer(SDL_Renderer* renderer, Player* player);
void cleanupPlayer(Player* player);
