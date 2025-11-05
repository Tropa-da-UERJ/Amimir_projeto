#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define NUM_BOTOES 3

typedef enum {
    ESTADO_MENU,
    ESTADO_JOGO,
    ESTADO_SAIR
} EstadoJogo;

// Estrutura para representar um botão
typedef struct {
    SDL_Rect retangulo; // Posição e dimensões do botão
    const char* texto;  // Texto do botão (opcional neste exemplo, mas bom para a estrutura)
} Botao;

SDL_Window* janela = NULL;
SDL_Renderer* renderizador = NULL;
EstadoJogo estado_atual = ESTADO_MENU;
int botao_selecionado = 0; // Índice do botão selecionado (0, 1 ou 2)

void desenhar_menu(Botao botoes[]);
void atualizar_menu(SDL_Event* evento, Botao botoes[]);
void executar_acao();
void fechar_sdl();

// --- Função Principal ---
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("SDL Teste", 
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              LARGURA_JANELA, ALTURA_JANELA, SDL_WINDOW_SHOWN);
    renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);

    // Criação dos botões
    Botao botoes[NUM_BOTOES];
    int largura_botao = 300;
    int altura_botao = 70;
    int espacamento = 20;
    int y_inicial = (ALTURA_JANELA - (NUM_BOTOES * altura_botao + (NUM_BOTOES - 1) * espacamento)) / 2;

    for (int i = 0; i < NUM_BOTOES; i++) {
        botoes[i].retangulo.w = largura_botao;
        botoes[i].retangulo.h = altura_botao;
        botoes[i].retangulo.x = (LARGURA_JANELA - largura_botao) / 2;
        botoes[i].retangulo.y = y_inicial + i * (altura_botao + espacamento);

        // Apenas para fins de visualização do estado:
        if (i == 0) botoes[i].texto = "INICIAR JOGO";
        else if (i == 1) botoes[i].texto = "OPCOES";
        else if (i == 2) botoes[i].texto = "SAIR";
    }

    // Loop Principal
    SDL_Event evento;
    bool rodando = true;

    while (rodando) {
        // Manipulação de Eventos
        while (SDL_PollEvent(&evento) != 0) {
            if (evento.type == SDL_QUIT) {
                rodando = false;
            }

            if (estado_atual == ESTADO_MENU) {
                atualizar_menu(&evento, botoes);
            }
        }

        // Lógica de Estado
        if (estado_atual == ESTADO_SAIR) {
            rodando = false;
        } else if (estado_atual == ESTADO_JOGO) {
            // Em uma aplicação real, aqui você chamaria a função de jogo
            SDL_Delay(500);
            printf("--- Jogo Iniciado! (Pressione ESC ou feche a janela para sair) ---\n");
            // Para sair do JOGO e voltar ao menu, você precisaria de mais lógica (ex: tecla ESC)
            // Para simplificar, vamos encerrar o loop principal no ESTADO_JOGO/ESTADO_SAIR.
            rodando = false;
        }

        // Renderização
        if (estado_atual == ESTADO_MENU) {
            SDL_SetRenderDrawColor(renderizador, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(renderizador);

            desenhar_menu(botoes);

            SDL_RenderPresent(renderizador);
        }
    }

    fechar_sdl();
    return 0;
}

void fechar_sdl() {
    if (renderizador != NULL) {
        SDL_DestroyRenderer(renderizador);
        renderizador = NULL;
    }
    if (janela != NULL) {
        SDL_DestroyWindow(janela);
        janela = NULL;
    }
    
    SDL_Quit();
    printf("SDL encerrado com sucesso.\n");
}

void desenhar_menu(Botao botoes[]) {
    // Desenha os botões
    for (int i = 0; i < NUM_BOTOES; i++) {
        if (i == botao_selecionado) {
            // Botão Selecionado: Cor Branca
            SDL_SetRenderDrawColor(renderizador, 0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            // Outros Botões: Cor Cinza
            SDL_SetRenderDrawColor(renderizador, 0x80, 0x80, 0x80, 0xFF);
        }

        // Desenha o retângulo do botão preenchido
        SDL_RenderFillRect(renderizador, &botoes[i].retangulo);
        
        // Opcional: Desenha um contorno para realçar o botão selecionado
        if (i == botao_selecionado) {
            SDL_SetRenderDrawColor(renderizador, 0xFF, 0x00, 0x00, 0xFF); 
        }

        // Em uma implementação real, você usaria a SDL_ttf para renderizar o texto dentro de cada retângulo.
        printf("Botao %d: %s (x:%d, y:%d, w:%d, h:%d) %s\n", i, botoes[i].texto, botoes[i].retangulo.x, botoes[i].retangulo.y, botoes[i].retangulo.w, botoes[i].retangulo.h, (i == botao_selecionado) ? "SELECIONADO" : "");
    }
}

void executar_acao() {
    printf("Acao selecionada: Botao %d (%s)\n", botao_selecionado, (botao_selecionado == 0) ? "INICIAR JOGO" : (botao_selecionado == 1) ? "OPCOES" : "SAIR");

    switch (botao_selecionado) {
        case 0: // INICIAR JOGO
            estado_atual = ESTADO_JOGO;
            break;
        case 1: // OPÇÕES (Neste exemplo, apenas imprime no console)
            printf("Opcoes... (nao implementado)\n");
            break;
        case 2: // SAIR
            estado_atual = ESTADO_SAIR;
            break;
    }
}

void atualizar_menu(SDL_Event* evento, Botao botoes[]) {
    switch (evento->type) {
        case SDL_KEYDOWN:
            switch (evento->key.keysym.sym) {
                case SDLK_UP:
                    botao_selecionado = (botao_selecionado - 1 + NUM_BOTOES) % NUM_BOTOES;
                    break;
                case SDLK_DOWN:
                    botao_selecionado = (botao_selecionado + 1) % NUM_BOTOES;
                    break;
                case SDLK_RETURN: // Tecla Enter
                case SDLK_KP_ENTER:
                    executar_acao();
                    break;
            }
            break;

        case SDL_MOUSEMOTION:
            for (int i = 0; i < NUM_BOTOES; i++) {
                // SDL_PointInRect verifica se o ponto (mouse x, mouse y) está dentro do SDL_Rect
                SDL_Point ponto_mouse = {evento->motion.x, evento->motion.y};
                if (SDL_PointInRect(&ponto_mouse, &botoes[i].retangulo)) {
                    botao_selecionado = i;
                    break;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (evento->button.button == SDL_BUTTON_LEFT) {
                // Verifica se o clique foi no botão atualmente selecionado (ou em qualquer um)
                for (int i = 0; i < NUM_BOTOES; i++) {
                    SDL_Point ponto_mouse = {evento->button.x, evento->button.y};
                    if (SDL_PointInRect(&ponto_mouse, &botoes[i].retangulo)) {
                        botao_selecionado = i; // Garante que o botão certo está selecionado
                        executar_acao();
                        break;
                    }
                }
            }
            break;
    }
}

