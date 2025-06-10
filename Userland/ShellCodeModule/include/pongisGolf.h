#include <stdint.h>
#include <math.h>
#include <library.h>
#include <stdarg.h>
#ifndef PONGISGOLS_H
#define PONGISGOLS_H

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#include <math.h>
#include <string.h>
#include <songs.h>

#define OK 1

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

#define PLAYER1_COLOR 0xFF9EA1 // rosa
#define PLAYER2_COLOR 0x00B4D2 // celeste
#define PLAYER_RADIUS 20
#define COLOR_WHITE 0xFFFFFF // Blanco
#define COLOR_BLACK 0x000000 // negro
#define COLOR_CYAN 0x9EFFDC
#define COLOR_TANGERINE 0xFF9933 // Rojo
#define BALL_RADIUS 10

#define BALL_HIT_SPEED  6.0f 
#define BALL_FRICTION   0.98f

#define MAX_LEVELS 5
#define MAX_HITS 3

#define ROTATION_SPEED     0.1f 
#define PLAYER_MAX_SPEED   8.0f
#define PLAYER_ACCELERATION 0.5f
#define PLAYER_DECELERATION 0.9f
#define M_PI 3.14159265358979323846264338327950288

void startPongisGolf();
void update_ball_position();
uint64_t isKeyPressed(uint8_t scancode);

typedef struct {
    float ball_x_ratio;    
    float ball_y_ratio;    
    float hole_x_ratio;    
    float hole_y_ratio;    
    int hole_radius;       
} LevelConfig;

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
} Ball;

typedef struct {
    float x, y;
    float radius;
} Hole;

typedef struct {
    float x, y;
    float radius;
    uint32_t color;
    float angle;        
    float speed;        
    uint8_t forward;    
    uint8_t backward;   
    uint8_t rotate_left;  
    uint8_t rotate_right;
} Player;

#endif