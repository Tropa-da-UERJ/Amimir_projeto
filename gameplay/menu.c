#include "menu.h" // Inclui seu próprio cabeçalho
#include <SDL_ttf.h>

// ... Structs e variáveis GLOBAIS *apenas* para o menu ...
static Button gBtnPlay;
static Button gBtnScoreboard;
static Button gBtnQuit;
static Button gMenuButtons[3];
static int gNumButtons = 3;
static TTF_Font* gFont = NULL;
static int gCurrentButtonIndex = 0;

void initMenu(SDL_Renderer* renderer) {
    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf não pôde inicializar! Erro: %s\n", TTF_GetError());
        return;
    }
    // Supondo que você tenha uma fonte "arial.ttf" no seu diretório
    gFont = TTF_OpenFont("arial.ttf", 48); 
    if (!gFont) {
        fprintf(stderr, "Falha ao carregar a fonte: %s\n", TTF_GetError());
        return;
    }

    // Cores
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    int center_x = 400; // Supondo janela de 800 de largura
    int start_y = 200;

    // 2. Configuração do botão "Play"
    gBtnPlay.texNormal = createTextTexture(renderer, gFont, "JOGAR", white);
    gBtnPlay.texSelected = createTextTexture(renderer, gFont, "JOGAR", yellow);
    gBtnPlay.targetState = GAME_STATE_PLAYING;
    // Define a posição e tamanho
    SDL_QueryTexture(gBtnPlay.texNormal, NULL, NULL, &gBtnPlay.rect.w, &gBtnPlay.rect.h);
    gBtnPlay.rect.x = center_x - gBtnPlay.rect.w / 2; // Centraliza X
    gBtnPlay.rect.y = start_y;
    gMenuButtons[0] = gBtnPlay;

    // 3. Configuração do botão "Scoreboard"
    gBtnScoreboard.texNormal = createTextTexture(renderer, gFont, "PLACAR", white);
    gBtnScoreboard.texSelected = createTextTexture(renderer, gFont, "PLACAR", yellow);
    gBtnScoreboard.targetState = GAME_STATE_SCOREBOARD;
    // Define a posição (abaixo de Play, com padding de 10px)
    SDL_QueryTexture(gBtnScoreboard.texNormal, NULL, NULL, &gBtnScoreboard.rect.w, &gBtnScoreboard.rect.h);
    gBtnScoreboard.rect.x = center_x - gBtnScoreboard.rect.w / 2;
    gBtnScoreboard.rect.y = gBtnPlay.rect.y + gBtnPlay.rect.h + 10; 
    gMenuButtons[1] = gBtnScoreboard;
    
    // 4. Configuração do botão "Sair"
    gBtnQuit.texNormal = createTextTexture(renderer, gFont, "SAIR", white);
    gBtnQuit.texSelected = createTextTexture(renderer, gFont, "SAIR", yellow);
    gBtnQuit.targetState = GAME_STATE_QUIT;
    // Define a posição (abaixo de Scoreboard)
    SDL_QueryTexture(gBtnQuit.texNormal, NULL, NULL, &gBtnQuit.rect.w, &gBtnQuit.rect.h);
    gBtnQuit.rect.x = center_x - gBtnQuit.rect.w / 2;
    gBtnQuit.rect.y = gBtnScoreboard.rect.y + gBtnScoreboard.rect.h + 10;
    gMenuButtons[2] = gBtnQuit;
}

void handleMenuInput(SDL_Event* event, GameState* currentState) {
    if (event->type == SDL_QUIT) {
        *currentState = GAME_STATE_QUIT;
        return;
    }

    // 2. Evento de Movimento do Mouse (Hover)
    if (event->type == SDL_MOUSEMOTION) {
        SDL_Point mousePos = {event->motion.x, event->motion.y};
        
        // Itera sobre todos os botões para verificar qual está sob o mouse
        for (int i = 0; i < gNumButtons; i++) {
            // SDL_PointInRect verifica se o ponto do mouse está dentro do retângulo do botão
            if (SDL_PointInRect(&mousePos, &gMenuButtons[i].rect)) {
                gMenuButtons[i].isSelected = true;
            } else {
                gMenuButtons[i].isSelected = false;
            }
        }
    }
    
    // 3. Evento de Clique do Mouse (Ação)
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            for (int i = 0; i < gNumButtons; i++) {
                if (gMenuButtons[i].isSelected) {
                    *currentState = gMenuButtons[i].targetState; 
                    return; 
                }
            }
        }
    }
    
    if (event->type == SDL_KEYDOWN) {
        for (int i = 0; i < gNumButtons; i++) {
            gMenuButtons[i].isSelected = false;
        }

        switch (event->key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                gCurrentButtonIndex--;
                if (gCurrentButtonIndex < 0) {
                    gCurrentButtonIndex = gNumButtons - 1;
                }
                break;

            case SDLK_DOWN:
            case SDLK_s:
                gCurrentButtonIndex++;
                if (gCurrentButtonIndex >= gNumButtons) {
                    gCurrentButtonIndex = 0;
                }
                break;

            case SDLK_RETURN:
                *currentState = gMenuButtons[gCurrentButtonIndex].targetState;
                return; // Sai após a ação
        }

        gMenuButtons[gCurrentButtonIndex].isSelected = true;
    }

void renderMenu(SDL_Renderer* renderer, Button* menuButtons, int numButtons /*, SDL_Texture* background */) {
    // 1. Limpar a tela
    SDL_RenderClear(renderer);

    // 2. (Opcional) Renderizar o fundo
    // SDL_RenderCopy(renderer, background, NULL, NULL);

    // 3. Renderizar os botões
    for (int i = 0; i < numButtons; i++) {
        if (menuButtons[i].isSelected) {
            // Desenha a textura selecionada
            SDL_RenderCopy(renderer, menuButtons[i].texSelected, NULL, &menuButtons[i].rect);
        } else {
            // Desenha a textura normal
            SDL_RenderCopy(renderer, menuButtons[i].texNormal, NULL, &menuButtons[i].rect);
        }
    }

    // 4. Apresentar o renderizador
    SDL_RenderPresent(renderer);
}

void cleanupMenu() {
    // TTF_CloseFont(gMenuFont);
    // SDL_DestroyTexture(gBtnJogar.texNormal);
    // ... etc ...
}


//typedef struct menu{
//  void (*on_open)(void);
//  void (*on_close)(void);
//  bool (*handle_input)(SDL_Event* event);
//} menu;
