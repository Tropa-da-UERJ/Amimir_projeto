#include "render.h"

#include "text.h"

// Cores que usaremos
SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 255};
SDL_Color red = {255, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color yellow = {255, 255, 0, 255};

void render(SDL_Renderer * renderer, App * app, Player player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], GameState currentState) {

  // --- 1. Desenha o Jogo (STATE_PLAYING ou STATE_LEVELUP) ---
  if (currentState == STATE_PLAYING || currentState == STATE_LEVELUP) {

    // Limpa a tela (fundo azul)
    SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255);
    SDL_RenderClear(renderer);

    // Desenha o Jogador
    if (player.invulnerable) {
      Uint32 now = SDL_GetTicks();
      if ((now / 100) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // roxo
      } else {
        SDL_SetRenderDrawColor(renderer, 50, 161, 157, 255); // cor do fundo
      }
    } else {
      SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // roxo normal
    }
    SDL_RenderFillRect(renderer, & player.rect);

    // Desenha os Inimigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active) {
        if (enemies[i].type == 0) {
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // ANSIEDADE
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // CAFEINA
        }
        SDL_RenderFillRect(renderer, & enemies[i].rect);
      }
    }

    // Desenha os Projéteis (Branco)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
      if (bullets[i].active) {
        SDL_RenderFillRect(renderer, & bullets[i].rect);
      }
    }

    // Desenha os Projéteis Inimigos (pretos)
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
      if (enemyBullets[i].active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // preto
        SDL_RenderFillRect(renderer, & enemyBullets[i].rect);
      }
    }

    // Desenhar a HUD (Health, Points, XP) ---
    if (app -> font) { // Só desenha se a fonte foi carregada
      char hudText[150];

      // 1. HUD Superior Esquerda (Vida e Pontos)
      sprintf(hudText, "HP: %d/%d | PONTOS: %d", player.hp, player.max_hp, player.points);
      drawText(renderer, app -> font, hudText, 10, 10, white);

      // 2. HUD Superior Direita (XP para o Próximo Nível)
      sprintf(hudText, "XP: %d / %d", player.xp, player.xp_toLevel);
      int textX = SCREEN_WIDTH - 200; // Começa 200 pixels da direita
      drawText(renderer, app -> font, hudText, textX, 10, yellow);
    }
  }
  // --- 2. Desenha o Menu (STATE_MENU) ---
  else if (currentState == STATE_MENU) {

    // Limpa a tela (fundo preto)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Verificamos a fonte aqui, mas a mensagem de erro é mais importante no drawText
    if (app -> font) {
      drawText(renderer, app -> font, "A MIMIR", 320, 200, white);
      drawText(renderer, app -> font, "Pressione ENTER para comecar", 200, 300, white);
      drawText(renderer, app -> font, "Setas/WASD para mover | Mire com o mouse", 130, 350, yellow);
    }
  }
  // --- 3. Desenha o Fim de Jogo (STATE_END) ---
  else if (currentState == STATE_END) {

    // Limpa a tela (fundo vermelho escuro)
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (app -> font) {
      char finalText[100];
      // Usar snprintf para evitar estouro de buffer ***
      snprintf(finalText, 100, "Voce fez %d pontos!", player.points);

      drawText(renderer, app -> font, "GAME OVER", 300, 200, white);
      drawText(renderer, app -> font, finalText, 250, 300, white);
      drawText(renderer, app -> font, "Pressione ENTER para voltar ao Menu", 150, 350, yellow);
    }
  }

  // --- 4. Desenha a Tela de Level Up (POR CIMA de STATE_PLAYING) ---
  if (currentState == STATE_LEVELUP) {
    //escurecer a tela
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // blend permite transparencia

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // cor preta semi-transparente

    SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, & screenRect);

    // Desenha as opções (no centro da tela)
    if (app -> font) {
      drawText(renderer, app -> font, "--- LEVEL UP! ---", 250, 150, white);
      drawText(renderer, app -> font, "Escolha seu upgrade! (Aperte 1, 2 ou 3...)", 250, 200, white);

      drawText(renderer, app -> font, "1. HP Maximo +5", 250, 250, red);
      drawText(renderer, app -> font, "2. Cadencia de Tiro +10%", 250, 280, red);
      drawText(renderer, app -> font, "3. Velocidade de Movimento +1", 250, 310, red);
    }
  }
  SDL_RenderPresent(renderer);
  }
