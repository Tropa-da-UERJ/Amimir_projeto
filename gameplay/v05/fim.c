#include "fim.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Texturas
static SDL_Texture* texFundoFim = NULL;
static SDL_Texture* texSplashGameOver = NULL;

// Texturas para o estado NORMAL
static SDL_Texture* texBotaoJogarNovamente = NULL;
static SDL_Texture* texBotaoVoltarMenu = NULL;

// Texturas para o estado SELECIONADO
static SDL_Texture* texBotaoJogarNovamente_H = NULL;
static SDL_Texture* texBotaoVoltarMenu_H = NULL;

static MenuButton botaoSelecionado = END_BUTTON_PLAYAGAIN;

// Posições
static SDL_Rect rectFundoFim;
static SDL_Rect rectSplashGameOver;
static SDL_Rect rectBotaoJogarNovamente;
static SDL_Rect rectBotaoVoltarMenu;

static void executar_acao_botao(GameState *currentState);
static bool estaNaSplash = true;

static SDL_Texture* carregarTextura(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* novaTextura = IMG_LoadTexture(renderer, path);
    if (novaTextura == NULL) {
        printf("Não foi possível carregar a textura '%s'! IMG_Error: %s\n", path, IMG_GetError());
    }
    return novaTextura;
}

static bool cliqueNoRetangulo(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void fim_init(SDL_Renderer *renderer) {
    // Carrega Asset do Fundo ---
    texFundoFim = carregarTextura("assets/fundo_fim_sprite.png", renderer);
    
    // O rect de fundo cobre toda a tela
    rectFundoFim.x = 0;
    rectFundoFim.y = 0;
    rectFundoFim.w = SCREEN_WIDTH;
    rectFundoFim.h = SCREEN_HEIGHT;

    // Carrega assets da Splash
    texSplashGameOver = carregarTextura("assets/splash_gameover.png", renderer);
    
    // Carrega assets do Menu
    texBotaoJogarNovamente = carregarTextura("assets/menu_botao_jogardnv.png", renderer);
    texBotaoVoltarMenu = carregarTextura("assets/menu_botao_voltarmenu.png", renderer);

    texBotaoJogarNovamente_H = carregarTextura("assets/menu_botao_jogardnv_H.png", renderer);
    texBotaoVoltarMenu_H = carregarTextura("assets/menu_botao_voltarmenu_H.png", renderer);

    // --- Define Posições (usando o tamanho real das texturas) ---
    int texW, texH;
    
    // Posições Menu
    const int ESPACAMENTO_BOTAO = 20;

    SDL_QueryTexture(texSplashGameOver, NULL, NULL, &texW, &texH);
    rectSplashGameOver = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 80, texW, texH };
    
    SDL_QueryTexture(texBotaoJogarNovamente, NULL, NULL, &texW, &texH);
    rectBotaoJogarNovamente = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 200, texW, texH };
    
    SDL_QueryTexture(texBotaoVoltarMenu, NULL, NULL, &texW, &texH);
    rectBotaoVoltarMenu = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, rectBotaoJogarNovamente.y + rectBotaoJogarNovamente.h + ESPACAMENTO_BOTAO, texW, texH };
    
    estaNaSplash = true; // Garante que começa na splash
    botaoSelecionado = END_BUTTON_PLAYAGAIN;
}

void fim_handle_input(SDL_Event *event, GameState *currentState) {

    if (event->type == SDL_MOUSEMOTION) {
            // Se o mouse se mover sobre um botão, ele o seleciona
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoJogarNovamente)) {
                botaoSelecionado = END_BUTTON_PLAYAGAIN;
            } else if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoVoltarMenu)) {
                botaoSelecionado = END_BUTTON_MENUAGAIN;
            }
    }
        
    if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
                // Navegação com Setas
                case SDLK_DOWN:
                    botaoSelecionado = (botaoSelecionado + 1) % END_BUTTON_COUNT;
                    break;
                case SDLK_s:
                    botaoSelecionado = (botaoSelecionado + 1) % END_BUTTON_COUNT;
                    break;
                case SDLK_UP:
                    botaoSelecionado = (botaoSelecionado - 1 + END_BUTTON_COUNT) % END_BUTTON_COUNT;
                    break;
                case SDLK_w:
                    botaoSelecionado = (botaoSelecionado - 1 + END_BUTTON_COUNT) % END_BUTTON_COUNT;
                    break;
                
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                case SDLK_SPACE:
                    executar_acao_botao(currentState);
                    break;
            }
    }
        
    if (event->type == SDL_MOUSEBUTTONDOWN) {
            // Se houver clique do mouse, a ação é executada no botão selecionado
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoJogarNovamente) ||
                cliqueNoRetangulo(mouseX, mouseY, rectBotaoVoltarMenu))
            {
                executar_acao_botao(currentState);
            }
    }

    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        *currentState = STATE_QUIT;
    }
}

static void executar_acao_botao(GameState *currentState) {
    switch (botaoSelecionado) {
        case END_BUTTON_PLAYAGAIN:
            *currentState = STATE_PLAYING;
            estaNaSplash = true; // Reseta o menu
            break;
        case END_BUTTON_MENUAGAIN:
            *currentState = STATE_MENU;
            printf("Voltando ao Menu...\n");
            break;
    }
}

void fim_render(SDL_Renderer *renderer) {
    // Desenha o Sprite de Fundo ---
    if (texFundoFim != NULL) {
        SDL_RenderCopy(renderer, texFundoFim, NULL, &rectFundoFim);
    } else {
        // Caso o sprite falhe, use a cor de fundo padrão para garantir que a tela limpe
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    // Desenha a tela de GameOver
    SDL_RenderCopy(renderer, texSplashGameOver, NULL, &rectSplashGameOver);

    // Botão Jogar
    if (botaoSelecionado == END_BUTTON_PLAYAGAIN) {
        SDL_RenderCopy(renderer, texBotaoJogarNovamente_H, NULL, &rectBotaoJogarNovamente);
    } else {
        SDL_RenderCopy(renderer, texBotaoJogarNovamente, NULL, &rectBotaoJogarNovamente);
    }

    // Botão Menu
    if (botaoSelecionado == END_BUTTON_MENUAGAIN) {
        SDL_RenderCopy(renderer, texBotaoVoltarMenu_H, NULL, &rectBotaoVoltarMenu);
    } else {
        SDL_RenderCopy(renderer, texBotaoVoltarMenu, NULL, &rectBotaoVoltarMenu);
    }
}

void fim_destroy(void) {
    // Libera todas as texturas
    SDL_DestroyTexture(texFundoFim);
    SDL_DestroyTexture(texSplashGameOver);
    SDL_DestroyTexture(texBotaoJogarNovamente);
    SDL_DestroyTexture(texBotaoVoltarMenu);
    SDL_DestroyTexture(texBotaoJogarNovamente_H);
    SDL_DestroyTexture(texBotaoVoltarMenu_H);
}

