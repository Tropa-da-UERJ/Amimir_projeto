#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// --- Constantes do Jogo ---
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define SQUARE_SIZE 30

#define PLAYER_SPEED 5
#define PLAYER_STARTING_HP 10
#define PLAYER_FIRE_DELAY 750  // ms entre tiros
#define PLAYER_BULLET_SPEED 10
#define PLAYER_MAX_BULLETS 200
#define PLAYER_XP_TOLEVEL 10
#define PLAYER_XP_LEVEL_SCALING 1.5f // proximo nivel custa 1.5x(50%) mais

#define BULLET_SIZE 8

#define INVULNERABILITY_DURATION 1000  // 1 segundo

#define MAX_ENEMIES 15
#define ENEMY_SPAWN_INTERVAL 800 // em milissegundos

#define ANXIETY_XP_GIVEN 2
#define ANXIETY_POINTS_GIVEN 10
#define ANXIETY_STARTING_HP 2
#define ANXIETY_SPEED 2

#define CAFEINA_XP_GIVEN 1
#define CAFEINA_POINTS_GIVEN 15
#define CAFEINA_STARTING_HP 1
#define CAFEINA_SPEED 1
#define CAFEINA_MAX_BULLETS 200
#define CAFEINA_BULLET_SPEED 10
#define CAFEINA_STOP_DISTANCE 300.0f
#define CAFEINA_FIRE_DELAY 2000

#define SEPARATION_STRENGTH 0.3f
#define MAX_OVERLAP 15

// --- Estados do Jogo ---
typedef enum {
    STATE_PLAYING,
    STATE_LEVELUP,
    STATE_MENU,
    STATE_END,
    STATE_PAUSED
} GameState;

// --- Estruturas de Dados ---

typedef struct {
    SDL_Rect rect;
    int dx, dy;
    int hp;
    int max_hp; // para o levelup
    Uint32 lastShotTime;
    bool invulnerable;
    Uint32 invulnerableTime;
    int xp;
    int xp_toLevel;
    int points;
    float speed;
    float fire_delay;
} Player;

typedef struct {
    SDL_Rect rect;
    bool active;
    int hp;
    int points_given;
    int xp_given;
    int type; // 0 para ANSIEDADE e 1 para CAFEINA
    int speed;
    Uint32 lastShotTime;
} Enemy;

typedef struct {
    SDL_Rect rect;
    int dx, dy;
    bool active;
} Bullet;

typedef struct {
    TTF_Font *font;
} App;

typedef struct {
    SDL_Rect rect;
    SDL_Texture* texNormal;
    SDL_Texture* texSelected;
    bool isSelected;
    GameState targetState; 
} Button;

#endif // DEFS_H
