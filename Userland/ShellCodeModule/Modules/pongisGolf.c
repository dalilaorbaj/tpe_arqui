#include <pongisGolf.h>

#define OK 1

// Constantes de alineación
#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

static void singlePlayer(uint32_t width, uint32_t height);
static void multiPlayer(uint32_t width, uint32_t height);
//void draw_ball(uint64_t x, uint64_t y, uint64_t radius, uint64_t color);
static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height);
static void printString2(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment);
static int basicPlay(Player *players, int player_count, float *ball_x, float *ball_y, float *ball_vx, float *ball_vy, float ball_radius, uint64_t ball_color, float hole_x, float hole_y, float hole_radius, uint64_t hole_color, uint32_t width, uint32_t height);
//int get_player_input(int player);
//void show_game_over_screen(int winner);

Screen screen;
// Calcular posición inicial y espaciado vertical
uint64_t baseY;  // Empezar a un tercio de la pantalla
uint64_t verticalSpacing;  // Espacio entre líneas (ajusta según necesites)
uint64_t currentY;


static void moveCurrentY(uint64_t verticalSpacing) {
    currentY += verticalSpacing;
}

void startPongisGolf(){
    puts("Starting Pongis Golf...");
    clearScreen();
    
    if(get_screen_info(&screen) <0 ){
        puts("Failed to get screen info.");
        return;
    }

    baseY = screen.height / 3;
    verticalSpacing = FONT_HEIGHT * 3;
    currentY = baseY;  // Iniciar desde un tercio de la pantalla

    draw_rectangle(0, 0, screen.width, screen.height, 0x00FF00); // Fill the screen with green
    //printText();


    printString2("Choose how many players: ", screen.width/2, currentY, 3, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing);

    //Opciones de menú
    printString2("1. Single Player", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);

    printString2("2. Multiplayer", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);
    printString2("3. Exit", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);

    printString2("Press '1' for Single Player or '2' for Multiplayer", screen.width / 2, currentY + 20, 1, screen.width, screen.height, ALIGN_CENTER);



    char choice;
    do {
        choice = getChar();
    
        if (choice == '1') {
            clearScreen();
            singlePlayer(screen.width, screen.height);
            return;
        } else if (choice == '2') {
            clearScreen();
            multiPlayer(screen.width, screen.height);
            return;
        } else if (choice == '3') {
            puts("Exiting Pongis Golf...");
            return;
        } else {
            // Opcional: mostrar mensaje de error
            printString("Invalid choice. Please select 1, 2, or 3.", screen.width / 2 - 10, screen.height - 30, 2, screen.width, screen.height);
        }
    } while (1); // Bucle infinito hasta que se elija una opción válida
    return;
}


static void printString2(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment) {
    if (str == NULL || y >= height) {
        return; // Invalid input
    }
    
    // Calcular la longitud de la cadena
    uint64_t str_length = strlen(str);
    
    // Ajustar la coordenada X según la alineación
    if (alignment == ALIGN_CENTER) {
        uint64_t string_width = str_length * FONT_WIDTH * size;
        x = (width - string_width) / 2;
    } 
    else if (alignment == ALIGN_RIGHT) {
        uint64_t string_width = str_length * FONT_WIDTH * size;
        x = width - string_width - x; // x actúa como margen derecho
    }
    
    // Verificar límites
    if (x >= width) return;
    
    // Dibujar el texto
    uint64_t i = 0;
    while (str[i] != '\0' && x + i * FONT_WIDTH * size < width) {
        draw_letter(x + i * FONT_WIDTH * size, y, str[i], 0xFF0000, size);
        i++;
    }
}

static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height) {
    if (str == NULL || x >= width || y >= height) {
        return; // Invalid input
    }
   // uint64_t centerX = x - strlen(str) * FONT_WIDTH * size;
    uint64_t str_length = strlen(str);
    uint64_t string_width = str_length * FONT_WIDTH * size;
    uint64_t centerX = (width - string_width) / 2;
    uint64_t i = 0;
    while (str[i] != '\0' && centerX + i * FONT_WIDTH * size < width) {
        draw_letter(centerX + i * FONT_WIDTH * size, y, str[i], 0xFF0000, size); // Draw each character in green
        i++;
    }
}

// static void singlePlayer(uint32_t width, uint32_t height) {
//     puts("Starting Single Player mode...");
//     clearScreen();

//     // Parámetros iniciales
//     float player_x = width / 4;
//     float player_y = height / 2;
//     float player_radius = 10;
//     uint64_t player_color = 0x0000FF; // Azul

//     float ball_x = width / 2;
//     float ball_y = height / 2;
//     float ball_radius = 8;
//     uint64_t ball_color = 0xFFFFFF; // Blanco

//     float ball_vx = 0, ball_vy = 0;
//     float ball_friction = 0.98f;

//     float hole_x = width * 3 / 4;
//     float hole_y = height / 2;
//     float hole_radius = 12;
//     uint64_t hole_color = 0x00FF00; // Verde

//     while (1) {
//         clearScreen();

//         // Dibujar hoyo
//         draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, hole_color);

//         // Dibujar pelota
//         draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)ball_radius, ball_color);

//         // Dibujar jugador
//         draw_ball((uint64_t)player_x, (uint64_t)player_y, (uint64_t)player_radius, player_color);

//         printString("Single Player Mode", width / 2 - 100, 30, 2);

//         // Input jugador
//         uint16_t key = 0;
//         if (sys_read(0, &key, 1) > 0) {
//             switch (key) {
//                 case 0x4800: // Flecha arriba
//                     if (player_y - 5 > player_radius)
//                         player_y -= 5;
//                     break;
//                 case 0x5000: // Flecha abajo
//                     if (player_y + 5 < height - player_radius)
//                         player_y += 5;
//                     break;
//                 case 0x4B00: // Flecha izquierda
//                     if (player_x - 5 > player_radius)
//                         player_x -= 5;
//                     break;
//                 case 0x4D00: // Flecha derecha
//                     if (player_x + 5 < width - player_radius)
//                         player_x += 5;
//                     break;
//                 case ' ': // Barra espaciadora para golpear
//                 {
//                     // Si el jugador está cerca de la pelota, golpearla
//                     float dx = ball_x - player_x;
//                     float dy = ball_y - player_y;
//                     float dist = sqrtf(dx*dx + dy*dy);
//                     if (dist < player_radius + ball_radius + 5) {
//                         // Golpe: darle velocidad a la pelota en la dirección del jugador
//                         float norm = sqrtf(dx*dx + dy*dy);
//                         if (norm != 0) {
//                             ball_vx += (dx / norm) * 8;
//                             ball_vy += (dy / norm) * 8;
//                         }
//                     }
//                     break;
//                 }
//             }
//         }

//         // Actualizar posición de la pelota
//         ball_x += ball_vx;
//         ball_y += ball_vy;
//         ball_vx *= ball_friction;
//         ball_vy *= ball_friction;

//         // Rebote con paredes
//         if (ball_x - ball_radius < 0 || ball_x + ball_radius > screen.width) {
//             ball_vx = -ball_vx;
//             // Corrige posición para no quedar fuera
//             if (ball_x - ball_radius < 0) ball_x = ball_radius;
//             if (ball_x + ball_radius > screen.width) ball_x = screen.width - ball_radius;
//         }
//         if (ball_y - ball_radius < 0 || ball_y + ball_radius > screen.height) {
//             ball_vy = -ball_vy;
//             if (ball_y - ball_radius < 0) ball_y = ball_radius;
//             if (ball_y + ball_radius > screen.height) ball_y = screen.height - ball_radius;
//         }

//         // Verificar si la pelota entra en el hoyo
//         float hx = ball_x - hole_x;
//         float hy = ball_y - hole_y;
//         if (sqrtf(hx*hx + hy*hy) < hole_radius) {
//             printString("¡Ganaste!", screen.width / 2 - 50, screen.height / 2, 3);
//             sys_nano_sleep(60);
//             break;
//         }

//         sys_nano_sleep(10);
//     }
// }

static int basicPlay(Player *players, int player_count, float *ball_x, float *ball_y, float *ball_vx, float *ball_vy, float ball_radius, uint64_t ball_color,
                     float hole_x, float hole_y, float hole_radius, uint64_t hole_color, uint32_t width, uint32_t height) {
    float ball_friction = 0.98f;
    int winner = -1;

    while (1) {
        clearScreen();

        // Dibujar hoyo
        draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, hole_color);

        // Dibujar pelota
        draw_ball((uint64_t)(*ball_x), (uint64_t)(*ball_y), (uint64_t)ball_radius, ball_color);

        // Dibujar jugadores
        for (int i = 0; i < player_count; i++) {
            draw_ball((uint64_t)players[i].x, (uint64_t)players[i].y, (uint64_t)players[i].radius, players[i].color);
        }

        //unsigned char sys_get_key(void);

        // Input jugadores
        uint16_t key = 0;
        if (sys_get_key > 0) {
            for (int i = 0; i < player_count; i++) {
                if (key == players[i].up && players[i].y - 5 > players[i].radius)
                    players[i].y -= 5;
                else if (key == players[i].down && players[i].y + 5 < height - players[i].radius)
                    players[i].y += 5;
                else if (key == players[i].left && players[i].x - 5 > players[i].radius)
                    players[i].x -= 5;
                else if (key == players[i].right && players[i].x + 5 < width - players[i].radius)
                    players[i].x += 5;
                else if (key == players[i].hit) {
                    // Si el jugador está cerca de la pelota, golpearla
                    float dx = (*ball_x) - players[i].x;
                    float dy = (*ball_y) - players[i].y;
                    float dist = (float)sqrtf(dx*dx + dy*dy);
                    if (dist < players[i].radius + ball_radius + 5) {
                        float norm = (float)sqrtf(dx*dx + dy*dy);
                        if (norm != 0) {
                            *ball_vx += (dx / norm) * 8;
                            *ball_vy += (dy / norm) * 8;
                        }
                    }
                }
            }
        }

        // Actualizar posición de la pelota
        *ball_x += *ball_vx;
        *ball_y += *ball_vy;
        *ball_vx *= ball_friction;
        *ball_vy *= ball_friction;

        // Rebote con paredes
        if (*ball_x - ball_radius < 0 || *ball_x + ball_radius > width) {
            *ball_vx = -*ball_vx;
            if (*ball_x - ball_radius < 0) *ball_x = ball_radius;
            if (*ball_x + ball_radius > width) *ball_x = width - ball_radius;
        }
        if (*ball_y - ball_radius < 0 || *ball_y + ball_radius > height) {
            *ball_vy = -*ball_vy;
            if (*ball_y - ball_radius < 0) *ball_y = ball_radius;
            if (*ball_y + ball_radius > height) *ball_y = height - ball_radius;
        }

        // Verificar si la pelota entra en el hoyo
        float hx = *ball_x - hole_x;
        float hy = *ball_y - hole_y;
        if (sqrtf(hx*hx + hy*hy) < hole_radius) {
            // Determinar qué jugador está más cerca del hoyo (opcional)
            int closest = 0;
            float min_dist = (float)sqrtf((players[0].x - *ball_x)*(players[0].x - *ball_x) + (players[0].y - *ball_y)*(players[0].y - *ball_y));
            for (int i = 1; i < player_count; i++) {
                float d = (float)sqrtf((players[i].x - *ball_x)*(players[i].x - *ball_x) + (players[i].y - *ball_y)*(players[i].y - *ball_y));
                if (d < min_dist) {
                    min_dist = d;
                    closest = i;
                }
            }
            winner = closest;
            break;
        }

        sys_nano_sleep(10);
    }
    return winner;
}

static void singlePlayer(uint32_t width, uint32_t height) {
    Player player = {width / 4, height / 2, 10, 0x0000FF, 0x4800, 0x5000, 0x4B00, 0x4D00, ' '};
    float ball_x = width / 2, ball_y = height / 2, ball_vx = 0, ball_vy = 0;
    float ball_radius = 8;
    uint64_t ball_color = 0xFFFFFF;
    float hole_x = width * 3 / 4, hole_y = height / 2, hole_radius = 12;
    uint64_t hole_color = 0x00FF00;

    basicPlay(&player, 1, &ball_x, &ball_y, &ball_vx, &ball_vy, ball_radius, ball_color, hole_x, hole_y, hole_radius, hole_color, width, height);

    printString("¡Ganaste!", width / 2 - 50, height / 2, 3, width, height);
    sys_nano_sleep(60);
}

static void multiPlayer(uint32_t width, uint32_t height) {
    Player players[2] = {
        {width / 4, height / 2, 10, 0x0000FF, 0x4800, 0x5000, 0x4B00, 0x4D00, ' '}, // Flechas
        {width * 3 / 4, height / 2, 10, 0xFF0000, 'w', 's', 'a', 'd', 'e'}           // WASD + 'e' para golpear
    };
    float ball_x = width / 2, ball_y = height / 2, ball_vx = 0, ball_vy = 0;
    float ball_radius = 8;
    uint64_t ball_color = 0xFFFFFF;
    float hole_x = width / 2, hole_y = height / 4, hole_radius = 12;
    uint64_t hole_color = 0x00FF00;

    int winner = basicPlay(players, 2, &ball_x, &ball_y, &ball_vx, &ball_vy, ball_radius, ball_color, hole_x, hole_y, hole_radius, hole_color, width, height);

    if (winner == 0)
        printString("¡Jugador 1 gana!", width / 2 - 80, height / 2, 3, width, height);
    else if (winner == 1)
        printString("¡Jugador 2 gana!", width / 2 - 80, height / 2, 3, width, height);
    else
        printString("¡Empate!", width / 2 - 50, height / 2, 3, width, height);
    
    sys_nano_sleep(60);
}