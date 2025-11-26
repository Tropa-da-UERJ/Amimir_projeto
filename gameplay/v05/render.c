#include "render.h"
#include "defs.h"
#include "menu.h"
#include "text.h"
#include "fim.h"

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
    if (player.texture != NULL) {
        Uint32 now = SDL_GetTicks();
        SDL_Rect srcRect;
        srcRect.x = player.frameX;
        srcRect.y = 0; // sheet horizontal
        srcRect.w = 32;
        srcRect.h = 32;

        if (player.invulnerable) {
            // lógica de piscar
            if ((now / 200) % 2 == 0) {
                 SDL_RenderCopy(renderer, player.texture, &srcRect, &player.rect); 
            }
        } else {
            SDL_RenderCopy(renderer, player.texture, &srcRect, &player.rect);
        }
    } else {
        // Fallback: Se o sprite não carregou, desenha o retângulo roxo padrão
        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); 
        SDL_RenderFillRect(renderer, &player.rect);
    }

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
    menu_render(renderer);

    if (menu_is_in_splash()) { // <-- Assumindo nova função auxiliar em menu.c/h
      Uint32 now = SDL_GetTicks();
      if ((now % 1000) < 500) { // Pisca a cada 1 segundo (500ms on, 500ms off)
        const char *flashText = "Pressione qualquer tecla para continuar";
        // Calculamos X para centralizar o texto (aproximação sem saber a largura exata)
        // O texto centralizado aparecerá em torno de 200, que é uma boa estimativa para o tamanho da tela:
        drawText(renderer, app->font, flashText, 200, 500, white);
      }
    }
  }

    // --- 3. Desenha o Fim de Jogo (STATE_END) ---
  else if (currentState == STATE_END) {
    fim_render(renderer, player, app);
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
