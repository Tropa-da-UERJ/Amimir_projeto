#include "menu.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Texturas
static SDL_Texture* texFundoMenu = NULL;
static SDL_Texture* texSplashNome = NULL;
static SDL_Texture* texSplashEnter = NULL;
static SDL_Texture* texMenuTitulo = NULL;

// Texturas para o estado NORMAL
static SDL_Texture* texBotaoJogar = NULL;
static SDL_Texture* texBotaoOpcoes = NULL;
static SDL_Texture* texBotaoSair = NULL;

// Texturas para o estado SELECIONADO
static SDL_Texture* texBotaoJogar_H = NULL;
static SDL_Texture* texBotaoOpcoes_H = NULL;
static SDL_Texture* texBotaoSair_H = NULL;

static MenuButton botaoSelecionado = MENU_BUTTON_PLAY;

// Posições
static SDL_Rect rectFundoMenu;
static SDL_Rect rectSplashNome;
static SDL_Rect rectSplashEnter;
static SDL_Rect rectMenuTitulo;
static SDL_Rect rectBotaoJogar;
static SDL_Rect rectBotaoOpcoes;
static SDL_Rect rectBotaoSair;

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

void menu_init(SDL_Renderer *renderer) {
    // Carrega Asset do Fundo ---
    texFundoMenu = carregarTextura("assets/fundo_menu_sprite.png", renderer);
    
    // O rect de fundo cobre toda a tela
    rectFundoMenu.x = 0;
    rectFundoMenu.y = 0;
    rectFundoMenu.w = SCREEN_WIDTH;
    rectFundoMenu.h = SCREEN_HEIGHT;

    // Carrega assets da Splash
    texSplashNome = carregarTextura("assets/splash_nome_jogo.png", renderer);
    texSplashEnter = carregarTextura("assets/splash_press_enter.png", renderer);
    
    // Carrega assets do Menu
    texMenuTitulo = carregarTextura("assets/menu_titulo.png", renderer);
    texBotaoJogar = carregarTextura("assets/menu_botao_jogar.png", renderer);
    texBotaoOpcoes = carregarTextura("assets/menu_botao_opcoes.png", renderer);
    texBotaoSair = carregarTextura("assets/menu_botao_sair.png", renderer);

    texBotaoJogar_H = carregarTextura("assets/menu_botao_jogar_H.png", renderer);
    texBotaoOpcoes_H = carregarTextura("assets/menu_botao_opcoes_H.png", renderer);
    texBotaoSair_H = carregarTextura("assets/menu_botao_sair_H.png", renderer);

    // --- Define Posições (usando o tamanho real das texturas) ---
    int texW, texH;
    
    // Posições Splash
    SDL_QueryTexture(texSplashNome, NULL, NULL, &texW, &texH);
    rectSplashNome = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 150, texW, texH };
    
    SDL_QueryTexture(texSplashEnter, NULL, NULL, &texW, &texH);
    rectSplashEnter = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 400, texW, texH };

    // Posições Menu
    const int ESPACAMENTO_BOTAO = 20;
    
    SDL_QueryTexture(texMenuTitulo, NULL, NULL, &texW, &texH);
    rectMenuTitulo = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 80, texW, texH };

    SDL_QueryTexture(texBotaoJogar, NULL, NULL, &texW, &texH);
    rectBotaoJogar = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, 200, texW, texH };
    
    SDL_QueryTexture(texBotaoOpcoes, NULL, NULL, &texW, &texH);
    rectBotaoOpcoes = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, rectBotaoJogar.y + rectBotaoJogar.h + ESPACAMENTO_BOTAO, texW, texH };
                                 
    SDL_QueryTexture(texBotaoSair, NULL, NULL, &texW, &texH);
    rectBotaoSair = (SDL_Rect){ (SCREEN_WIDTH - texW) / 2, rectBotaoOpcoes.y + rectBotaoOpcoes.h + ESPACAMENTO_BOTAO, texW, texH };
    
    estaNaSplash = true; // Garante que começa na splash
    botaoSelecionado = MENU_BUTTON_PLAY;
}

void menu_handle_input(SDL_Event *event, GameState *currentState) {
    // 1. Inputs da Tela Splash
    if (estaNaSplash) {
        if (event->type == SDL_KEYDOWN && (event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_KP_ENTER)) {
            estaNaSplash = false; // Sai da splash, vai para o menu principal
        }
    }
    // 2. Inputs do Menu Principal (Mouse)
    else {
        if (event->type == SDL_MOUSEMOTION) {
            // Se o mouse se mover sobre um botão, ele o seleciona
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoJogar)) {
                botaoSelecionado = MENU_BUTTON_PLAY;
            } else if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoOpcoes)) {
                botaoSelecionado = MENU_BUTTON_OPTIONS;
            } else if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoSair)) {
                botaoSelecionado = MENU_BUTTON_QUIT;
            }
        }
        
        if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
                // Navegação com Setas
                case SDLK_DOWN:
                    botaoSelecionado = (botaoSelecionado + 1) % MENU_BUTTON_COUNT;
                    break;
                case SDLK_UP:
                    botaoSelecionado = (botaoSelecionado - 1 + MENU_BUTTON_COUNT) % MENU_BUTTON_COUNT;
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
            
            if (cliqueNoRetangulo(mouseX, mouseY, rectBotaoJogar) ||
                cliqueNoRetangulo(mouseX, mouseY, rectBotaoOpcoes) ||
                cliqueNoRetangulo(mouseX, mouseY, rectBotaoSair)) 
            {
                executar_acao_botao(currentState);
            }
        }
    }

    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        *currentState = STATE_QUIT;
    }
}

static void executar_acao_botao(GameState *currentState) {
    switch (botaoSelecionado) {
        case MENU_BUTTON_PLAY:
            *currentState = STATE_PLAYING;
            estaNaSplash = true; // Reseta o menu
            break;
        case MENU_BUTTON_OPTIONS:
            *currentState = STATE_OPTIONS;
            printf("Abrindo tela de Opções...\n");
            break;
        case MENU_BUTTON_QUIT:
            *currentState = STATE_QUIT;
            break;
        case MENU_BUTTON_COUNT:
            break;
    }
}

void menu_render(SDL_Renderer *renderer) {
    // Desenha o Sprite de Fundo ---
    if (texFundoMenu != NULL) {
        SDL_RenderCopy(renderer, texFundoMenu, NULL, &rectFundoMenu);
    } else {
        // Caso o sprite falhe, use a cor de fundo padrão para garantir que a tela limpe
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    if (estaNaSplash) {
        // Desenha a tela de Splash
        SDL_RenderCopy(renderer, texSplashNome, NULL, &rectSplashNome);
        SDL_RenderCopy(renderer, texSplashEnter, NULL, &rectSplashEnter);
    } else {
        // Desenha o Menu Principal
        SDL_RenderCopy(renderer, texMenuTitulo, NULL, &rectMenuTitulo);
        
        // Botão Jogar
        if (botaoSelecionado == MENU_BUTTON_PLAY) {
            SDL_RenderCopy(renderer, texBotaoJogar_H, NULL, &rectBotaoJogar);
        } else {
            SDL_RenderCopy(renderer, texBotaoJogar, NULL, &rectBotaoJogar);
        }

        // Botão Opções
        if (botaoSelecionado == MENU_BUTTON_OPTIONS) {
            SDL_RenderCopy(renderer, texBotaoOpcoes_H, NULL, &rectBotaoOpcoes);
        } else {
            SDL_RenderCopy(renderer, texBotaoOpcoes, NULL, &rectBotaoOpcoes);
        }

        // Botão Sair
        if (botaoSelecionado == MENU_BUTTON_QUIT) {
            SDL_RenderCopy(renderer, texBotaoSair_H, NULL, &rectBotaoSair);
        } else {
            SDL_RenderCopy(renderer, texBotaoSair, NULL, &rectBotaoSair);
        }
    }
}

void menu_destroy(void) {
    // Libera todas as texturas
    SDL_DestroyTexture(texFundoMenu);
    SDL_DestroyTexture(texSplashNome);
    SDL_DestroyTexture(texSplashEnter);
    SDL_DestroyTexture(texMenuTitulo);
    SDL_DestroyTexture(texBotaoJogar);
    SDL_DestroyTexture(texBotaoOpcoes);
    SDL_DestroyTexture(texBotaoSair);
    SDL_DestroyTexture(texBotaoJogar_H);
    SDL_DestroyTexture(texBotaoOpcoes_H);
    SDL_DestroyTexture(texBotaoSair_H);
}

