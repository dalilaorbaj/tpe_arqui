#include <stdint.h>
#include <math.h>
#include <library.h>
#ifndef PONGISGOLS_H
#define PONGISGOLS_H

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#include <math.h>
#include <string.h>

#define OK 1

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

#define PLAYER_1_INIT_X 64
#define PLAYER_1_INIT_Y 94

#define PLAYER1_COLOR 0xFF9EA1 // violeta
#define PLAYER2_COLOR 0x00B4D2 // celeste
#define PLAYER_RADIUS 10
#define BALL_COLOR 0xFFFFFF // Blanco
#define HOLE_COLOR 0x000000 // negro
#define BALL_RADIUS 8
#define HOLE_RADIUS 12

#define PLAYER_SPEED    8.0f
#define BALL_HIT_SPEED  6.0f 
#define BALL_FRICTION   0.98f

#define MAX_LEVELS 5

#define SCANCODE_W 17
#define SCANCODE_S 31  
#define SCANCODE_A 30
#define SCANCODE_D 32
#define SCANCODE_I 23
#define SCANCODE_K 37
#define SCANCODE_J 36
#define SCANCODE_L 38
#define EXIT_KEY '0' // Tecla para salir del juego


void startPongisGolf();
void update_ball_position();
int isKeyPressed(uint8_t scancode);

// Configuración de niveles
typedef struct {
    float ball_x_ratio;    // Posición X de la pelota como ratio del ancho de pantalla
    float ball_y_ratio;    // Posición Y de la pelota como ratio del alto de pantalla
    float hole_x_ratio;    // Posición X del hoyo como ratio del ancho de pantalla
    float hole_y_ratio;    // Posición Y del hoyo como ratio del alto de pantalla
    int hole_radius;       // Radio del hoyo para este nivel
} LevelConfig;

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
} Ball;
//vx y vy para las velocidades


typedef struct {
    float x, y;
    float radius;
} Hole;

typedef struct {
    float x, y;
    float radius;
    uint64_t color;
    char up, down, left, right; // teclas de control
} Player;

#endif