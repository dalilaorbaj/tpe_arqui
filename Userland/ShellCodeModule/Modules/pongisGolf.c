
#include <pongisGolf.h>

// Configuraciones de los 5 niveles
static LevelConfig levels[MAX_LEVELS] = {
    // Nivel 1: Como está ahora - pelota centro, hoyo arriba centro
    {0.5f, 0.5f, 0.5f, 0.25f, 40},
    
    // Nivel 2: Pelota alineada con hoyo pero más lejos
    {0.5f, 0.8f, 0.5f, 0.2f, 30},
    
    // Nivel 3: Pelota desalineada, hoyo en otra posición
    {0.3f, 0.7f, 0.7f, 0.3f, 20},
    
    // Nivel 4: Pelota en borde lateral, hoyo en esquina opuesta
    {0.1f, 0.5f, 0.8f, 0.2f, 15},
    
    // Nivel 5: Pelota en esquina, hoyo en esquina opuesta
    {0.1f, 0.1f, 0.9f, 0.9f, 12}
};

static void singlePlayer(uint32_t width, uint32_t height);
static void multiPlayer(uint32_t width, uint32_t height);
static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment);
static void processPlayerMovements(Player *players, uint32_t width, uint32_t height);
static void flushKeyboardBuffer();
static void setExit();

Screen screen;
uint64_t baseY;
uint64_t verticalSpacing;
uint64_t currentY;
uint64_t screenWidth;
uint64_t screenHeight;

static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment) {
    if (str == NULL || y >= height) {
        return; 
    }
    
    uint64_t str_length = strlen(str);
    
    if (alignment == ALIGN_CENTER) {
        uint64_t string_width = str_length * FONT_WIDTH * size;
        x = (width - string_width) / 2;
    } 
    else if (alignment == ALIGN_RIGHT) {
        uint64_t string_width = str_length * FONT_WIDTH * size;
        x = width - string_width - x;
    }
    
    if (x >= width) return;
    
    uint64_t i = 0;
    while (str[i] != '\0' && x + i * FONT_WIDTH * size < width) {
        draw_letter(x + i * FONT_WIDTH * size, y, str[i], 0xFF7F50, size);
        i++;
    }
}

static void moveCurrentY(uint64_t verticalSpacing) {
    currentY += verticalSpacing;
}

void startPongisGolf(){
    puts("Starting Pongis Golf...");

    if(get_screen_info(&screen) <0 ){
        puts("Failed to get screen info.");
        return;
    }

    baseY = screen.height / 3;
    verticalSpacing = FONT_HEIGHT * 3;
    currentY = baseY;
    screenWidth = screen.width;
    screenHeight = screen.height;

    draw_rectangle(0, 0, screen.width, screen.height, 0x9EFFDC);

    printString("Choose how many players: ", screen.width/2, currentY, 3, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing);

    printString("1. Single Player", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);

    printString("2. Multiplayer", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);
    printString("3. Exit", screen.width / 2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);

    printString("Press '1' for Single Player or '2' for Multiplayer", screen.width / 2, currentY + 20, 1, screen.width, screen.height, ALIGN_CENTER);

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
            setExit();
            return;
        } else {
            printString("Invalid choice. Please select 1, 2, or 3.", screen.width / 2, screen.height - 30, 2, screen.width, screen.height, ALIGN_CENTER);
        }
    } while (1);
    return;
}

static void singlePlayer(uint32_t width, uint32_t height){
    return;
}

static void showLevelInfo(int level, uint32_t width, uint32_t height) {
    clearScreen();
    draw_rectangle(0, 0, width, height, 0x9EFFDC);
    
    if (level == 0) {
        printString("LEVEL 1", width/2, height/2 - 60, 4, width, height, ALIGN_CENTER);
        printString("Hole size: 40", width/2, height/2, 2, width, height, ALIGN_CENTER);
        printString("Easy start - get ready!", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
    } else if (level == 1) {
        printString("LEVEL 2", width/2, height/2 - 60, 4, width, height, ALIGN_CENTER);
        printString("Hole size: 30", width/2, height/2, 2, width, height, ALIGN_CENTER);
        printString("Getting harder...", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
    } else if (level == 2) {
        printString("LEVEL 3", width/2, height/2 - 60, 4, width, height, ALIGN_CENTER);
        printString("Hole size: 20", width/2, height/2, 2, width, height, ALIGN_CENTER);
        printString("Getting harder...", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
    } else if (level == 3) {
        printString("LEVEL 4", width/2, height/2 - 60, 4, width, height, ALIGN_CENTER);
        printString("Hole size: 15", width/2, height/2, 2, width, height, ALIGN_CENTER);
        printString("Getting harder...", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
    } else if (level == 4) {
        printString("LEVEL 5", width/2, height/2 - 60, 4, width, height, ALIGN_CENTER);
        printString("Hole size: 12", width/2, height/2, 2, width, height, ALIGN_CENTER);
        printString("FINAL LEVEL - Maximum difficulty!", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
    }
    
    printString("Starting in 2 seconds...", width/2, height/2 + 80, 2, width, height, ALIGN_CENTER);
    sys_nano_sleep(50); // 2 segundos
}

static void multiPlayer(uint32_t width, uint32_t height) {
    int current_level = 0;
    int player1_wins = 0;
    int player2_wins = 0; 
    
    while (current_level < MAX_LEVELS) {
        // Mostrar información del nivel
        showLevelInfo(current_level, width, height);
        
        Player players[2] = {
            {width / 4, height/2, PLAYER_RADIUS, PLAYER1_COLOR, 'w', 's', 'a', 'd'},
            {width * 3 / 4, height/2, PLAYER_RADIUS, PLAYER2_COLOR, 'i', 'k', 'j', 'l'}           
        };
        
        // Configurar posiciones según el nivel actual
        LevelConfig config = levels[current_level];
        float ball_x = width * config.ball_x_ratio;
        float ball_y = height * config.ball_y_ratio;
        float ball_vx = 0, ball_vy = 0;
        float hole_x = width * config.hole_x_ratio;
        float hole_y = height * config.hole_y_ratio;
        int hole_radius = config.hole_radius;

        int last_hit = -1;
        int winner = -1;
        uint8_t buffer;
        uint64_t nbytes;
        
        // Variables para guardar posiciones previas de los jugadores y la pelota
        float prev_player_x[2] = {players[0].x, players[1].x};
        float prev_player_y[2] = {players[0].y, players[1].y};
        float prev_ball_x = ball_x;
        float prev_ball_y = ball_y;
        
        clearScreen();
        
        // Mostrar controles
        printString("Player 1: w=up, s=down, a=left, d=right", width/2, height/2 - 40, 2, width, height, ALIGN_CENTER);
        printString("Player 2: i=up, k=down, j=left, l=right", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
        
        printString("Starting in 1 second...", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
        sys_nano_sleep(40); // 1 segundo
        
        // Dibujo inicial completo
        draw_rectangle(0, 0, width, height, 0x9EFFDC);
        draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, HOLE_COLOR);
        draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, BALL_COLOR);
        for (int i = 0; i < 2; i++) {
            draw_ball((uint64_t)players[i].x, (uint64_t)players[i].y, (uint64_t)players[i].radius, players[i].color);
        }

        // Game loop para este nivel
        while (1) {
            // Guardar posiciones previas antes de procesar movimientos
            for (int i = 0; i < 2; i++) {
                prev_player_x[i] = players[i].x;
                prev_player_y[i] = players[i].y;
            }
            prev_ball_x = ball_x;
            prev_ball_y = ball_y;
            
            // uint8_t tecla = getChar();
            if(isKeyPressed(SCANCODE_ESCAPE)) { // 'ESC'
                setExit();
                return;
            }

            processPlayerMovements(players, width, height);

            // Detectar colisión jugador–pelota con física mejorada
            for (int i = 0; i < 2; i++) {
                float dx = ball_x - players[i].x;
                float dy = ball_y - players[i].y;
                float dist2 = dx*dx + dy*dy;
                float minDist = players[i].radius + BALL_RADIUS;
                if (dist2 <= minDist*minDist) {
                    // Calcular velocidad del jugador
                    float player_vx = players[i].x - prev_player_x[i];
                    float player_vy = players[i].y - prev_player_y[i];
                    
                    // Si el jugador se está moviendo, usar su dirección de movimiento
                    if (player_vx != 0.0f || player_vy != 0.0f) {
                        float speed = sqrtf(player_vx*player_vx + player_vy*player_vy);
                        if (speed > 0.0f) {
                            ball_vx = (player_vx / speed) * BALL_HIT_SPEED;
                            ball_vy = (player_vy / speed) * BALL_HIT_SPEED;
                        }
                    } else {
                        // Si el jugador está quieto, usar la dirección desde el centro del jugador hacia la pelota
                        float norm = sqrtf(dx*dx + dy*dy);
                        if (norm == 0.0f) norm = 1.0f;
                        ball_vx = (dx / norm) * BALL_HIT_SPEED;
                        ball_vy = (dy / norm) * BALL_HIT_SPEED;
                    }
                    
                    // Separar la pelota del jugador para evitar colisiones múltiples
                    float separation_factor = (minDist + 2.0f) / sqrtf(dist2);
                    ball_x = players[i].x + dx * separation_factor;
                    ball_y = players[i].y + dy * separation_factor;
                    
                    last_hit = i;
                }
            }

            // Actualizar posición y velocidad de la pelota
            ball_x += ball_vx;
            ball_y += ball_vy;
            ball_vx *= BALL_FRICTION;
            ball_vy *= BALL_FRICTION;

            // Rebotes contra bordes
            if (ball_x < BALL_RADIUS) {
                ball_x = BALL_RADIUS;
                ball_vx = -ball_vx * 0.5f;
            }
            if (ball_x > width - BALL_RADIUS) {
                ball_x = width - BALL_RADIUS;
                ball_vx = -ball_vx * 0.5f;
            }
            if (ball_y < BALL_RADIUS) {
                ball_y = BALL_RADIUS;
                ball_vy = -ball_vy * 0.5f;
            }
            if (ball_y > height - BALL_RADIUS) {
                ball_y = height - BALL_RADIUS;
                ball_vy = -ball_vy * 0.5f;
            }

            // Verificar si la pelota entra en el hoyo
            float dxh = ball_x - hole_x;
            float dyh = ball_y - hole_y;
            float dist2h = dxh*dxh + dyh*dyh;
            if (dist2h <= hole_radius*hole_radius) {
                winner = (last_hit >= 0) ? last_hit : 0;
                play_victory_sound();
                break;
            }
            
            // PINTADO SELECTIVO - Borrar posiciones anteriores si son diferentes
            // Primero borrar la pelota anterior si se movió
            if ((uint64_t)prev_ball_x != (uint64_t)ball_x || (uint64_t)prev_ball_y != (uint64_t)ball_y) {
                draw_ball((uint64_t)prev_ball_x, (uint64_t)prev_ball_y, (uint64_t)BALL_RADIUS, 0x9EFFDC);
            }
            
            // Borrar jugadores anteriores si se movieron
            for (int i = 0; i < 2; i++) {
                if ((uint64_t)prev_player_x[i] != (uint64_t)players[i].x || (uint64_t)prev_player_y[i] != (uint64_t)players[i].y) {
                    draw_ball((uint64_t)prev_player_x[i], (uint64_t)prev_player_y[i], (uint64_t)players[i].radius, 0x9EFFDC);
                }
            }
            
            // Redibujar el hoyo si está en la misma posición que algún elemento que se borró
            int redraw_hole = 0;
            float hole_dist_threshold = hole_radius + BALL_RADIUS + 5;
            
            // Verificar si el hoyo se superpone con la posición anterior de la pelota
            if ((uint64_t)prev_ball_x != (uint64_t)ball_x || (uint64_t)prev_ball_y != (uint64_t)ball_y) {
                float dx_hole_ball = prev_ball_x - hole_x;
                float dy_hole_ball = prev_ball_y - hole_y;
                if (dx_hole_ball*dx_hole_ball + dy_hole_ball*dy_hole_ball < hole_dist_threshold*hole_dist_threshold) {
                    redraw_hole = 1;
                }
            }
            
            // Verificar si el hoyo se superpone con la posición anterior de algún jugador
            for (int i = 0; i < 2; i++) {
                if ((uint64_t)prev_player_x[i] != (uint64_t)players[i].x || (uint64_t)prev_player_y[i] != (uint64_t)players[i].y) {
                    float dx_hole_player = prev_player_x[i] - hole_x;
                    float dy_hole_player = prev_player_y[i] - hole_y;
                    float player_hole_threshold = hole_radius + players[i].radius + 5;
                    if (dx_hole_player*dx_hole_player + dy_hole_player*dy_hole_player < player_hole_threshold*player_hole_threshold) {
                        redraw_hole = 1;
                    }
                }
            }
            
            if (redraw_hole) {
                draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, HOLE_COLOR);
            }
            
            // Dibujar nuevas posiciones
            draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, BALL_COLOR);
            for (int i = 0; i < 2; i++) {
                draw_ball((uint64_t)players[i].x, (uint64_t)players[i].y, (uint64_t)players[i].radius, players[i].color);
            }

            // Pequeña pausa para controlar la velocidad del juego
            sys_nano_sleep(1);
        }

        // Mostrar resultado del nivel
        clearScreen();
        draw_rectangle(0, 0, width, height, 0x9EFFDC);
        
        if (winner == 0) {
            player1_wins++;
            printString("Player 1 scores!", width/2, height/2 - 20, 3, width, height, ALIGN_CENTER);
        } else {
            player2_wins++;
            printString("Player 2 scores!", width/2, height/2 - 20, 3, width, height, ALIGN_CENTER);
        }

        
        // Mostrar score usando printString en lugar de sprintf
        //(!)esto no es de buen estilo, mas adelante arreglarlo
        if (player1_wins == 0 && player2_wins == 0) {
            printString("Score - Player 1: 0  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 1 && player2_wins == 0) {
            printString("Score - Player 1: 1  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 0 && player2_wins == 1) {
            printString("Score - Player 1: 0  Player 2: 1", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 1 && player2_wins == 1) {
            printString("Score - Player 1: 1  Player 2: 1", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 2 && player2_wins == 0) {
            printString("Score - Player 1: 2  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 0 && player2_wins == 2) {
            printString("Score - Player 1: 0  Player 2: 2", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 2 && player2_wins == 1) {
            printString("Score - Player 1: 2  Player 2: 1", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 1 && player2_wins == 2) {
            printString("Score - Player 1: 1  Player 2: 2", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 2 && player2_wins == 2) {
            printString("Score - Player 1: 2  Player 2: 2", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 3 && player2_wins == 0) {
            printString("Score - Player 1: 3  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 0 && player2_wins == 3) {
            printString("Score - Player 1: 0  Player 2: 3", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 3 && player2_wins == 1) {
            printString("Score - Player 1: 3  Player 2: 1", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 1 && player2_wins == 3) {
            printString("Score - Player 1: 1  Player 2: 3", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 3 && player2_wins == 2) {
            printString("Score - Player 1: 3  Player 2: 2", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 2 && player2_wins == 3) {
            printString("Score - Player 1: 2  Player 2: 3", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 3 && player2_wins == 3) {
            printString("Score - Player 1: 3  Player 2: 3", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 4 && player2_wins == 0) {
            printString("Score - Player 1: 4  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 0 && player2_wins == 4) {
            printString("Score - Player 1: 0  Player 2: 4", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 4 && player2_wins == 1) {
            printString("Score - Player 1: 4  Player 2: 1", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 1 && player2_wins == 4) {
            printString("Score - Player 1: 1  Player 2: 4", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 4 && player2_wins == 2) {
            printString("Score - Player 1: 4  Player 2: 2", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 2 && player2_wins == 4) {
            printString("Score - Player 1: 2  Player 2: 4", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 4 && player2_wins == 3) {
            printString("Score - Player 1: 4  Player 2: 3", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 3 && player2_wins == 4) {
            printString("Score - Player 1: 3  Player 2: 4", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 4 && player2_wins == 4) {
            printString("Score - Player 1: 4  Player 2: 4", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 5 && player2_wins == 0) {
            printString("Score - Player 1: 5  Player 2: 0", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else if (player1_wins == 0 && player2_wins == 5) {
            printString("Score - Player 1: 0  Player 2: 5", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        } else {
            printString("Score updated", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
        }
        sys_nano_sleep(40); // 1 segundo

        // Avanzar al siguiente nivel
        current_level++;
        
        if (current_level < MAX_LEVELS) {
            if (current_level == 1) {
                printString("Advancing to Level 2...", width/2, height/2 + 60, 2, width, height, ALIGN_CENTER);
            } else if (current_level == 2) {
                printString("Advancing to Level 3...", width/2, height/2 + 60, 2, width, height, ALIGN_CENTER);
            } else if (current_level == 3) {
                printString("Advancing to Level 4...", width/2, height/2 + 60, 2, width, height, ALIGN_CENTER);
            } else if (current_level == 4) {
                printString("Advancing to Level 5...", width/2, height/2 + 60, 2, width, height, ALIGN_CENTER);
            }
            printString("Press any key to continue", width/2, height/2 + 100, 1, width, height, ALIGN_CENTER);
        } else {
            printString("ALL LEVELS COMPLETED!", width/2, height/2 + 60, 3, width, height, ALIGN_CENTER);
            
            // Determinar ganador final
            if (player1_wins > player2_wins) {
                printString("PLAYER 1 IS THE CHAMPION!", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
            } else if (player2_wins > player1_wins) {
                printString("PLAYER 2 IS THE CHAMPION!", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
            } else {
                printString("IT'S A TIE! Both players are champions!", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
            }
            
            printString("Press any key to return to main menu...", width/2, height/2 + 140, 1, width, height, ALIGN_CENTER);
        }
        sys_nano_sleep(40); // 1 segundo
        getChar();
        clearScreen();
    }
}

// Función para limpiar el buffer del teclado
static void flushKeyboardBuffer() {
    uint8_t buffer;
    uint64_t nbytes;
    
    // Lee y descarta todas las teclas acumuladas en el buffer
    while ((nbytes = sys_read(0, &buffer, 1)) > 0) {
        // Simplemente descartamos el valor leído
        // El bucle continúa hasta que no haya más datos en el buffer
    }
}

static void setExit(){
    clearScreen();
    printString("Exiting Pongis Golf...", screenWidth/2, screenHeight/2, 2, screenWidth, screenHeight, ALIGN_CENTER);
    sys_nano_sleep(40); // 1 segundo
    clearScreen();
    flushKeyboardBuffer();
}

static void processPlayerMovements(Player *players, uint32_t width, uint32_t height) {
    // Jugador 1 (WASD) - Todos los if son independientes
    if (isKeyPressed(SCANCODE_W)) {        // 'w'
        if (players[0].y - PLAYER_SPEED > players[0].radius) {
            players[0].y -= PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_S)) {  // 's'
        if (players[0].y + PLAYER_SPEED < height - players[0].radius) {
            players[0].y += PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_A)) {  // 'a'
        if (players[0].x - PLAYER_SPEED > players[0].radius) {
            players[0].x -= PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_D)) { // 'd'
        if (players[0].x + PLAYER_SPEED < width - players[0].radius) {
            players[0].x += PLAYER_SPEED;
        }
    }

    // Jugador 2 (IJKL) - Todos los if son independientes
    if (isKeyPressed(SCANCODE_I)) {    // 'i'
        if (players[1].y - PLAYER_SPEED > players[1].radius) {
            players[1].y -= PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_K)) {  // 'k'
        if (players[1].y + PLAYER_SPEED < height - players[1].radius) {
            players[1].y += PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_J)) {  // 'j'
        if (players[1].x - PLAYER_SPEED > players[1].radius) {
            players[1].x -= PLAYER_SPEED;
        }
    }
    else if (isKeyPressed(SCANCODE_L)) { // 'l'
        if (players[1].x + PLAYER_SPEED < width - players[1].radius) {
            players[1].x += PLAYER_SPEED;
        }
    }
}