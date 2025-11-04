#include "player.h"
#include <SDL_image.h>


Player createPlayer(SDL_Renderer* renderer, const char* texturePath, int startX, int startY) {
    Player newPlayer;
    
    SDL_Surface* surface = IMG_Load(texturePath);
    if (!surface) {
        // Lidar com erro
        fprintf(stderr, "Erro ao carregar Player Surface: %s\n", IMG_GetError());
        newPlayer.texture = NULL;
    } else {
        newPlayer.texture = SDL_CreateTextureFromSurface(renderer, surface);
        newPlayer.rect.w = surface->w; // Largura da textura
        newPlayer.rect.h = surface->h; // Altura da textura
        SDL_FreeSurface(surface);
    }
    
    newPlayer.x_pos = (float)startX;
    newPlayer.y_pos = (float)startY;
    newPlayer.x_vel = 0.0f;
    newPlayer.y_vel = 0.0f;
    newPlayer.health = 100; // Exemplo
    newPlayer.score = 0;
    
    // Define a posição inicial do rect (será atualizada em updatePlayer)
    newPlayer.rect.x = startX;
    newPlayer.rect.y = startY;

    return newPlayer;
}

void handlePlayerInput(Player* player, const Uint8* keyboardState) {
    // Redefine a velocidade
    player->x_vel = 0.0f;
    player->y_vel = 0.0f;
    
    float speed = 5.0f; // Velocidade de movimento
    
    if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) {
        player->y_vel = -speed;
    }
    if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) {
        player->y_vel = speed;
    }
    if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
        player->x_vel = -speed;
    }
    if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
        player->x_vel = speed;
    }
    
    // (Opcional) Normalizar a velocidade se estiver movendo na diagonal
    if (player->x_vel != 0.0f && player->y_vel != 0.0f) {
        // Usa teorema de pitágoras para manter a velocidade constante na diagonal
        // sqrt(2) ≈ 1.414
        player->x_vel /= 1.414f;
        player->y_vel /= 1.414f;
    }
}

void updatePlayer(Player* player) {
    // 1. Aplica a velocidade à posição
    player->x_pos += player->x_vel;
    player->y_pos += player->y_vel;
    
    // 2. Garante que o jogador permaneça dentro dos limites da tela (clamp)
    // (Ajuste MAX_X/MAX_Y para o tamanho da sua janela)
    player->x_pos = clamp(player->x_pos, 0, SCREEN_WIDTH - player->rect.w);
    player->y_pos = clamp(player->y_pos, 0, SCREEN_HEIGHT - player->rect.h);
    
    // 3. Atualiza o SDL_Rect para a renderização e colisão
    player->rect.x = (int)player->x_pos;
    player->rect.y = (int)player->y_pos;
    
    // (Aqui você adicionaria lógica de tiro, etc.)
}

void renderPlayer(SDL_Renderer* renderer, const Player* player) {
    if (player->texture) {
        SDL_RenderCopy(renderer, player->texture, NULL, &player->rect);
    }
    // (Opcional) Adicione código para desenhar a barra de vida ou outros elementos do jogador
}

void cleanupPlayer(Player* player) {
    if (player->texture) {
        SDL_DestroyTexture(player->texture);
        player->texture = NULL;
    }
}
