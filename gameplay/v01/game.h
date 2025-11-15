#pragma once // Evita inclusão duplicada

// Esta é a definição global dos estados do jogo
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_SCOREBOARD,
    GAME_STATE_PAUSED, // Você pode adicionar mais
    GAME_STATE_QUIT
} GameState;

// Funções do "jogo" (gameplay)
void initGame();
void handleGameInput();
void updateGame();
void renderGame();
void cleanupGame();
