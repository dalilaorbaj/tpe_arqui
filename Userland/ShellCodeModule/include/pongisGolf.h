#include <stdint.h>
#include <math.h>
#include <library.h>
#ifndef PONGISGOLS_H
#define PONGISGOLS_H

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void startPongisGolf();
void update_ball_position();


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