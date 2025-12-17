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

// Função auxiliar para pegar o texto do upgrade
const char* getUpgradeLabel(UpgradeType type) {
    switch (type) {
        case UPGRADE_HP_MAX: return "HP Maximo +5";
        case UPGRADE_FIRE_RATE: return "Cadencia de Tiro +10%";
        case UPGRADE_MOVE_SPEED: return "Velocidade de Movimento +1";
        case UPGRADE_DAMAGE: return "Dano Base +10%";
        case UPGRADE_MULTISHOT: return "Multishot (Tiro Multiplo)";
        case UPGRADE_BULLET_SPEED: return "Velocidade do Projetil +15%";
        case UPGRADE_DODGE: return "Esquiva (Dodge) +10%";
        default: return "???";
    }
}

void render(SDL_Renderer * renderer, App * app, Player player, Enemy enemies[], Bullet bullets[], Bullet enemyBullets[], GameState currentState) {

  // Desenha o Jogo (STATE_PLAYING ou STATE_LEVELUP) ---
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
          if (app->texAnsiedade != NULL) {
            // Desenha a sprite no retângulo que já tem o tamanho correto (escalado)
            SDL_RenderCopy(renderer, app->texAnsiedade, NULL, &enemies[i].rect);
          } else {
            // Fallback (Mantém o quadrado vermelho se a textura falhar)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
            SDL_RenderFillRect(renderer, &enemies[i].rect);
          }
        }
        else {
          if (app->texCafeina != NULL) {
                // Desenha a imagem esticada no tamanho do retângulo do inimigo
                SDL_RenderCopy(renderer, app->texCafeina, NULL, &enemies[i].rect);
          } else {
              SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
              SDL_RenderFillRect(renderer, &enemies[i].rect);
          }
        }
      }
    }

    // Desenha os Projéteis (Branco)
    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < PLAYER_MAX_BULLETS; i++) {
      if (bullets[i].active) {
        if (app->texBullet != NULL) {
            // Desenha a textura do Z
            SDL_RenderCopy(renderer, app->texBullet, NULL, &bullets[i].rect);
        } else {
            // Fallback: Se a textura falhar, desenha o quadrado antigo
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &bullets[i].rect);
        }
      }  
    }

    // Desenha os Projéteis Inimigos (pretos)
    for (int i = 0; i < CAFEINA_MAX_BULLETS; i++) {
      if (enemyBullets[i].active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // preto
        SDL_RenderFillRect(renderer, &enemyBullets[i].rect);
      }
    }

    if (app->texDarknessLayer != NULL && app->texSpotlight != NULL) {
        
        // Define a texture de luz como o ALVO
        SDL_SetRenderTarget(renderer, app->texDarknessLayer);

        // Calcula a luz ambiente baseada no timer
        
        int ambientLight = 255 - app->currentDarknessAlpha; 
        if (ambientLight < 20) ambientLight = 20; // Limite mínimo de escuridão total

        // Limpa a tela com a cor do ambiente (Cinza Escuro se for noite, Branco se for dia)
        SDL_SetRenderDrawColor(renderer, ambientLight, ambientLight, ambientLight, 255);
        SDL_RenderClear(renderer);

        // Desenha a "Lanterna" (soma luz branca) onde o player está  
        SDL_Rect lightRect;

        lightRect.w = SPOTLIGHT_RADIUS * 2; 
        lightRect.h = SPOTLIGHT_RADIUS * 2; 
        lightRect.x = player.rect.x + (player.rect.w / 2) - (lightRect.w / 2);
        lightRect.y = player.rect.y + (player.rect.h / 2) - (lightRect.h / 2);

        // Desenha a luz apagando a escuridão
        SDL_RenderCopy(renderer, app->texSpotlight, NULL, &lightRect);

        // Reseta o ALVO para a tela padrão
        SDL_SetRenderTarget(renderer, NULL);

        // Desenha a camada de escuridão pronta por cima do jogo
        SDL_RenderCopy(renderer, app->texDarknessLayer, NULL, NULL);
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

      // 3. HUD Superior Central (Timer)
      int remainingMs = ROUND_DURATION_MS - app->elapsedTime;

      if (remainingMs < 0)
        remainingMs = 0;

      int minutes = remainingMs / 60000;
      int seconds = (remainingMs % 60000) / 1000;

      char timerText[20]; 
      sprintf(timerText, "%d:%02d", minutes, seconds);
      drawText(renderer, app -> font, timerText, (SCREEN_WIDTH / 2) - 50, 10, yellow);
    }
  }
  // --- 2. Desenha o Menu (STATE_MENU) ---
  else if (currentState == STATE_MENU) {
    menu_render(renderer);

    if (menu_is_in_splash()) { // função auxiliar em menu.c/h
      Uint32 now = SDL_GetTicks();
      if ((now % 1000) < 500) { // pisca a cada 1 segundo (500ms on, 500ms off)
        const char *flashText = "Pressione qualquer tecla para continuar";
        drawText(renderer, app->font, flashText, 200, 400, white);
      }
    }
  }

    // --- 3. Desenha o Fim de Jogo (STATE_END) ---
  else if (currentState == STATE_END) {
    fim_render(renderer, player, app);
  }

  // --- TELA DE LEVEL UP ATUALIZADA ---
  if (currentState == STATE_LEVELUP) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Mais escuro para ler melhor
    SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, & screenRect);

    if (app->font) {
      drawText(renderer, app->font, "--- LEVEL UP! ---", 280, 100, white);
      drawText(renderer, app->font, "Escolha um upgrade (1, 2 ou 3):", 220, 150, white);

      // Renderiza as 3 opções sorteadas
      char optionText[100];
      
      sprintf(optionText, "1. %s", getUpgradeLabel(app->currentUpgradeOptions[0]));
      drawText(renderer, app->font, optionText, 150, 220, red);

      sprintf(optionText, "2. %s", getUpgradeLabel(app->currentUpgradeOptions[1]));
      drawText(renderer, app->font, optionText, 150, 270, red);

      sprintf(optionText, "3. %s", getUpgradeLabel(app->currentUpgradeOptions[2]));
      drawText(renderer, app->font, optionText, 150, 320, red);
    }
  }
  SDL_RenderPresent(renderer);
}
