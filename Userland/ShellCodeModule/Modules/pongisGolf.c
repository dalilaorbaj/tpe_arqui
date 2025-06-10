#include <pongisGolf.h>

static LevelConfig levels[MAX_LEVELS] = {
    // Nivel 1: Pelota centro, hoyo arriba centro
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

static void mainMenu();
static void singlePlayer(uint32_t width, uint32_t height);
static void multiPlayer(uint32_t width, uint32_t height);
static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment);
static void processPlayerMovements(Player *players, uint32_t width, uint32_t height);
static void resolvePlayerCollisions(Player *players, uint32_t width, uint32_t height);
static void printStringf(uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment, const char *format, ...);
static void setExit();
static void printCurrentLevelInfo();
static void printSinglePlayerInfo(int attempts, int max_attempts, int hits);
static int is_stopped(float vx, float vy);
static void draw_player(Player *player);
static float my_sin(float x);
static float my_cos(float x);
static float my_fabs(float x);

Screen screen;
uint64_t screenWidth;
uint64_t screenHeight;
uint64_t numberOfPlayers;
LevelConfig config;
int current_level, player1_wins, player2_wins;


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
        draw_letter(x + i * FONT_WIDTH * size, y, str[i], COLOR_TANGERINE, size);
        i++;
    }
}

void startPongisGolf(){
    puts("Starting Pongis Golf...");

    if(sys_get_screen_info(&screen) <0 ){
        puts("Failed to get screen info.");
        return;
    }

    screenWidth = screen.width;
    screenHeight = screen.height;

    mainMenu();
    flushKeyboardBuffer();
    return;
}

static void mainMenu(){
    flushKeyboardBuffer();
    draw_rectangle(0, 0, screenWidth, screenHeight, COLOR_CYAN);
    printString("Pongis Golf", screenWidth / 2, screenHeight / 2 - 200, 5, screenWidth, screenHeight, ALIGN_CENTER);

    printString("Choose how many players: ", screenWidth/2, screenHeight/2 - 60, 3, screenWidth, screenHeight, ALIGN_CENTER);

    printString("1. Single Player", screenWidth / 2, screenHeight/2, 2, screenWidth, screenHeight, ALIGN_CENTER);

    printString("2. Multiplayer", screenWidth / 2, screenHeight/2 + 40, 2, screenWidth, screenHeight, ALIGN_CENTER);

    printString("3. Exit", screenWidth / 2, screenHeight/2 + 80, 2, screenWidth, screenHeight, ALIGN_CENTER);

    char choice;
    do {
        choice = getChar();
    
        if (choice == '1') {
            clearScreen();
            singlePlayer(screenWidth, screenHeight);
            return;
        } else if (choice == '2') {
            clearScreen();
            multiPlayer(screenWidth, screenHeight);
            return;
        } else if (choice == '3') {
            setExit();
            return;
        } else {
            printString("Invalid choice. Please select 1, 2, or 3.", screenWidth / 2, screenHeight - 30, 2, screenWidth, screenHeight, ALIGN_CENTER);
        }
    } while (1);
}


static void showLevelInfo(int level, uint32_t width, uint32_t height) {
    clearScreen();
    draw_rectangle(0, 0, width, height, COLOR_CYAN);
    
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
    sys_nano_sleep(50);
}

static void singlePlayer(uint32_t width, uint32_t height) {
    numberOfPlayers = 1;
    current_level = 0; // Reiniciar nivel
    player1_wins = 0;
    
    while (current_level < MAX_LEVELS) {
        showLevelInfo(current_level, width, height);

        // Solo un jugador en el centro
        Player players[1] = {
            {width / 2, height/2 + 80, PLAYER_RADIUS, PLAYER1_COLOR,
            0.0f,    // ángulo inicial (apuntando a la derecha)
            0.0f,    // velocidad inicial
            SCANCODE_W, SCANCODE_S, SCANCODE_A, SCANCODE_D}
        };
        
        // Configurar posiciones según el nivel actual
        config = levels[current_level];
        float ball_x = width * config.ball_x_ratio;
        float ball_y = height * config.ball_y_ratio;
        float ball_vx = 0, ball_vy = 0;
        float hole_x = width * config.hole_x_ratio;
        float hole_y = height * config.hole_y_ratio;
        int hole_radius = config.hole_radius;

        int attempts = 0;
        int max_attempts = 10;
        
        float prev_player_x = players[0].x;
        float prev_player_y = players[0].y;
        float prev_ball_x = ball_x;
        float prev_ball_y = ball_y;
        
        clearScreen();
        
        // Mostrar controles en single player
        printString("Player controls: w=forward, s=back, a=rotate left, d=rotate right", width/2, height/2 - 40, 2, width, height, ALIGN_CENTER);
        printStringf(width/2, height/2 + 40, 2, width, height, ALIGN_CENTER, "Attempts remaining: %d", max_attempts);
        printString("Starting in 1 second...", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);

        sys_nano_sleep(40);
        
        // Dibujo inicial
        draw_rectangle(0, 0, width, height, COLOR_CYAN);
        draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, COLOR_BLACK);
        draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, COLOR_WHITE);
        draw_player(&players[0]);

        // Game loop para este nivel
        int level_completed = 0;
        while (!level_completed && attempts < max_attempts) {
            int attempt_success = 0;
            int ballHits = 0;
            
            while (!attempt_success && !level_completed) {
                prev_player_x = players[0].x;
                prev_player_y = players[0].y;
                prev_ball_x = ball_x;
                prev_ball_y = ball_y;
                
                if(isKeyPressed(SCANCODE_ESCAPE)) {
                    mainMenu();
                    return;
                }

                processPlayerMovements(&players[0], width, height);

                // Detectar colisión jugador–pelota
                float dx = ball_x - players[0].x;
                float dy = ball_y - players[0].y;
                float dist2 = dx*dx + dy*dy;
                float minDist = players[0].radius + BALL_RADIUS;
                if (dist2 <= minDist*minDist) {
                    ballHits++;

                    // velocidad del jugador
                    float player_vx = players[0].x - prev_player_x;
                    float player_vy = players[0].y - prev_player_y;
                    
                    // si el jugador se está moviendo, usamos su dirección de movimiento
                    if (player_vx != 0.0f || player_vy != 0.0f) {
                        float speed = sqrtf(player_vx*player_vx + player_vy*player_vy);
                        if (speed > 0.0f) {
                            ball_vx = (player_vx / speed) * BALL_HIT_SPEED;
                            ball_vy = (player_vy / speed) * BALL_HIT_SPEED;
                        }
                    } else {
                        // y si el jugador está quieto, usamos la dirección desde el centro del jugador hacia la pelota
                        float norm = sqrtf(dx*dx + dy*dy);
                        if (norm == 0.0f) norm = 1.0f;
                        ball_vx = (dx / norm) * BALL_HIT_SPEED;
                        ball_vy = (dy / norm) * BALL_HIT_SPEED;
                    }
                    float separation_factor = (minDist + 3.0f) / sqrtf(dist2);
                    ball_x = players[0].x + dx * separation_factor;
                    ball_y = players[0].y + dy * separation_factor;
                }

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

                // chequeamos si pelota entra en el hoyo
                float dxh = ball_x - hole_x;
                float dyh = ball_y - hole_y;
                float dist2h = dxh*dxh + dyh*dyh;
                if (dist2h <= hole_radius*hole_radius) {
                    level_completed = 1;
                    player1_wins++;
                    play_victory_sound();
                    break;
                }
                
                //Chequeaamo si se le terminaron los intentos
                if((ballHits == MAX_HITS && is_stopped(ball_vx, ball_vy)) || ballHits > MAX_HITS) {
                    attempt_success = 1;
                    attempts++;
                }
                
                // Rendering selectivo
                if ((uint64_t)prev_ball_x != (uint64_t)ball_x || (uint64_t)prev_ball_y != (uint64_t)ball_y) {
                    draw_ball((uint64_t)prev_ball_x, (uint64_t)prev_ball_y, (uint64_t)BALL_RADIUS, COLOR_CYAN);
                }
                
                if ((uint64_t)prev_player_x != (uint64_t)players[0].x || (uint64_t)prev_player_y != (uint64_t)players[0].y) {
                    draw_ball((uint64_t)prev_player_x, (uint64_t)prev_player_y, (uint64_t)players[0].radius, COLOR_CYAN);
                }
                
                int redraw_hole = 0;
                float hole_dist_threshold = hole_radius + BALL_RADIUS + 5;
                
                // chequeamos si el hoyo se superpone con la posición anterior de la pelota
                if ((uint64_t)prev_ball_x != (uint64_t)ball_x || (uint64_t)prev_ball_y != (uint64_t)ball_y) {
                    float dx_hole_ball = prev_ball_x - hole_x;
                    float dy_hole_ball = prev_ball_y - hole_y;
                    if (dx_hole_ball*dx_hole_ball + dy_hole_ball*dy_hole_ball < hole_dist_threshold*hole_dist_threshold) {
                        redraw_hole = 1;
                    }
                }
                
                // chequeamos si el hoyo se superpone con la posición anterior del jugador
                if ((uint64_t)prev_player_x != (uint64_t)players[0].x || (uint64_t)prev_player_y != (uint64_t)players[0].y) {
                    float dx_hole_player = prev_player_x - hole_x;
                    float dy_hole_player = prev_player_y - hole_y;
                    float player_hole_threshold = hole_radius + players[0].radius + 5;
                    if (dx_hole_player*dx_hole_player + dy_hole_player*dy_hole_player < player_hole_threshold*player_hole_threshold) {
                        redraw_hole = 1;
                    }
                }
                
                if (redraw_hole) {
                    draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, COLOR_BLACK);
                }
                
                // nuevas posiciones
                draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, COLOR_WHITE);
                draw_player(&players[0]);

                printSinglePlayerInfo(attempts, max_attempts, ballHits);

                sys_nano_sleep(1);
            }
            
            // reiniciar posiciones
            if (attempt_success && !level_completed) {
                ball_x = width * config.ball_x_ratio;
                ball_y = height * config.ball_y_ratio;
                ball_vx = 0;
                ball_vy = 0;
                
                // mensaje de intento fallido
                clearScreen();
                draw_rectangle(0, 0, width, height, COLOR_CYAN);
                printStringf(width/2, height/2, 3, width, height, ALIGN_CENTER, "Attempt %d failed!", attempts);
                printStringf(width/2, height/2 + 40, 2, width, height, ALIGN_CENTER, "Attempts remaining: %d", max_attempts - attempts);
                
                if (attempts < max_attempts) {
                    sys_nano_sleep(40); // Esperar antes de reiniciar
                    
                    // Redibujar campo
                    draw_rectangle(0, 0, width, height, COLOR_CYAN);
                    draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, COLOR_BLACK);
                    draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, COLOR_WHITE);

                    //reinicializar jugador
                    players[0].x = width / 2;
                    players[0].y = height / 2 + 80;
                    players[0].angle = 0.0f;
                    players[0].speed = 0.0f;
                    draw_player(&players[0]);
                }
            }
        }

        // Mostrar resultado del nivel
        clearScreen();
        draw_rectangle(0, 0, width, height, COLOR_CYAN);
        
        if (level_completed) {
            printStringf(width/2, height/2 - 20, 3, width, height, ALIGN_CENTER, "Level %d completed!", current_level + 1);
            printStringf(width/2, height/2 + 20, 2, width, height, ALIGN_CENTER, "Attempts used: %d/%d", attempts, max_attempts);
        } else {
            printStringf(width/2, height/2 - 20, 3, width, height, ALIGN_CENTER, "Level %d failed!", current_level + 1);
            printString("All attempts used. Game Over!", width/2, height/2 + 20, 2, width, height, ALIGN_CENTER);
            
            // volver al menú
            printString("Press any key to return to main menu...", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
            getChar();
            mainMenu();
            return;
        }
        
        printString("Press 'esc' to return to main menu", width/2, height/2 + 100, 1, width, height, ALIGN_CENTER);
        printString("Press 'enter' to go to the next level", width/2, height/2 + 120, 1, width, height, ALIGN_CENTER);
        while (1) {
            if (isKeyPressed(SCANCODE_ESCAPE)) {
                mainMenu();
                flushKeyboardBuffer();
                return;
            }
            else if( isKeyPressed(SCANCODE_ENTER)) {
                break;
            }
        }
        clearScreen();

        sys_nano_sleep(10); 

        // siguiente nivel
        current_level++;
        
        if (current_level < MAX_LEVELS) {
            printStringf(width/2, height/2 + 60, 2, width, height, ALIGN_CENTER, "Advancing to Level %d...", current_level + 1);
        } else {
            printString("ALL LEVELS COMPLETED!", width/2, height/2 + 60, 3, width, height, ALIGN_CENTER);
            printString("CONGRATULATIONS! You are a champion!", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);
        }

        sys_nano_sleep(40); 

        clearScreen();
    }
    
    // Volver al menú principal
    mainMenu();
}

static void multiPlayer(uint32_t width, uint32_t height) {
    numberOfPlayers = 2; 
    current_level = 0;
    player1_wins = 0;
    player2_wins = 0;
    
    while (current_level < MAX_LEVELS) {
        showLevelInfo(current_level, width, height);

        Player players[2] = {
            {
                width / 4, height/2, PLAYER_RADIUS, PLAYER1_COLOR,
                0.0f,  
                0.0f,   
                SCANCODE_W, SCANCODE_S, SCANCODE_A, SCANCODE_D
            },
            {
                width * 3 / 4, height/2, PLAYER_RADIUS, PLAYER2_COLOR,
                M_PI,   
                0.0f, 
                SCANCODE_I, SCANCODE_K, SCANCODE_J, SCANCODE_L
            }
        };
        
        config = levels[current_level];
        float ball_x = width * config.ball_x_ratio;
        float ball_y = height * config.ball_y_ratio;
        float ball_vx = 0, ball_vy = 0;
        float hole_x = width * config.hole_x_ratio;
        float hole_y = height * config.hole_y_ratio;
        int hole_radius = config.hole_radius;

        int last_hit = -1;
        int winner = -1;
        
        float prev_player_x[2] = {players[0].x, players[1].x};
        float prev_player_y[2] = {players[0].y, players[1].y};
        float prev_ball_x = ball_x;
        float prev_ball_y = ball_y;
        
        clearScreen();
        
        // controles para multiplayer
        printString("Player 1: w=up, s=down, a=rotate left, d= rotate right", width/2, height/2 - 40, 2, width, height, ALIGN_CENTER);
        printString("Player 2: i=up, k=down, j=rotate left, l=rotate right", width/2, height/2 + 40, 2, width, height, ALIGN_CENTER);
        printString("Starting in 1 second...", width/2, height/2 + 100, 2, width, height, ALIGN_CENTER);

        sys_nano_sleep(40);
        
        draw_rectangle(0, 0, width, height, COLOR_CYAN);
        draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, COLOR_BLACK);
        draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, COLOR_WHITE);
        for (int i = 0; i < 2; i++) {
            draw_ball((uint64_t)players[i].x, (uint64_t)players[i].y, (uint64_t)players[i].radius, players[i].color);
        }

        // Game loop
        while (1) {
            for (int i = 0; i < 2; i++) {
                prev_player_x[i] = players[i].x;
                prev_player_y[i] = players[i].y;
            }
            prev_ball_x = ball_x;
            prev_ball_y = ball_y;
            
            // vuelve al menú principal si se presiona 'ESC'
            if(isKeyPressed(SCANCODE_ESCAPE)) { 
                mainMenu();
                return;
            }

            processPlayerMovements(players, width, height);

            resolvePlayerCollisions(players, width, height);

            // Detectar colisión jugador–pelota
            for (int i = 0; i < 2; i++) {
                float dx = ball_x - players[i].x;
                float dy = ball_y - players[i].y;
                float dist2 = dx*dx + dy*dy;
                float minDist = players[i].radius + BALL_RADIUS;
                if (dist2 <= minDist*minDist) {
                    // velocidad del jugador
                    float player_vx = players[i].x - prev_player_x[i];
                    float player_vy = players[i].y - prev_player_y[i];
                    
                    // Si el jugador se mueve, usamos su dirección de movimiento
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
                draw_ball((uint64_t)prev_ball_x, (uint64_t)prev_ball_y, (uint64_t)BALL_RADIUS, COLOR_CYAN);
            }
            
            // Borrar jugadores anteriores si se movieron
            for (int i = 0; i < 2; i++) {
                if ((uint64_t)prev_player_x[i] != (uint64_t)players[i].x || (uint64_t)prev_player_y[i] != (uint64_t)players[i].y) {
                    draw_ball((uint64_t)prev_player_x[i], (uint64_t)prev_player_y[i], (uint64_t)players[i].radius, COLOR_CYAN);
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
                draw_ball((uint64_t)hole_x, (uint64_t)hole_y, (uint64_t)hole_radius, COLOR_BLACK);
            }
            
            // Dibujar nuevas posiciones
            draw_ball((uint64_t)ball_x, (uint64_t)ball_y, (uint64_t)BALL_RADIUS, COLOR_WHITE);
            for (int i = 0; i < 2; i++) {
                draw_player(&players[i]);
            }

            printCurrentLevelInfo();

            // Pequeña pausa para controlar la velocidad del juego
            sys_nano_sleep(1);
        }

        // Mostrar resultado del nivel
        clearScreen();
        draw_rectangle(0, 0, width, height, COLOR_CYAN);
        
        if (winner == 0) {
            player1_wins++;
            printString("Player 1 scores!", width/2, height/2 - 20, 3, width, height, ALIGN_CENTER);
        } else {
            player2_wins++;
            printString("Player 2 scores!", width/2, height/2 - 20, 3, width, height, ALIGN_CENTER);
        }

        
        printStringf(width/2, height/2 + 20, 2, width, height, ALIGN_CENTER, "Score - Player 1: %d  Player 2: %d", player1_wins, player2_wins);
        sys_nano_sleep(40); // 1 segundo

        // Avanzar al siguiente nivel
        current_level++;
        
        if (current_level < MAX_LEVELS) {
            printStringf(width/2, height/2 + 60, 2, width, height, ALIGN_CENTER, "Advancing to Level %d...", current_level + 1);

            printString("Press 'esc' to return to main menu", width/2, height/2 + 100, 1, width, height, ALIGN_CENTER);
            printString("Press 'enter' to go to the next level", width/2, height/2 + 120, 1, width, height, ALIGN_CENTER);

        } else {
            printString("ALL LEVELS COMPLETED!", width/2, height/2 + 60, 3, width, height, ALIGN_CENTER);
            
            // Determinar ganador final
            if (player1_wins > player2_wins) {
                printString("PLAYER 1 IS THE CHAMPION!", width/2, height/2 + 105, 2, width, height, ALIGN_CENTER);
            } else if (player2_wins > player1_wins) {
                printString("PLAYER 2 IS THE CHAMPION!", width/2, height/2 + 105, 2, width, height, ALIGN_CENTER);
            } else {
                printString("IT'S A TIE! Both players are champions!", width/2, height/2 + 105, 2, width, height, ALIGN_CENTER);
            }
            
            printString("Press 'esc' to return to main menu...", width/2, height/2 + 140, 1, width, height, ALIGN_CENTER);


        }

       
        while (1) {
            if (isKeyPressed(SCANCODE_ESCAPE)) {
                mainMenu();
                flushKeyboardBuffer();
                return;
            }
            else if(isKeyPressed(SCANCODE_ENTER) && current_level < MAX_LEVELS) {
                break;
            }
        }
        clearScreen();
    }
}


static void setExit(){
    clearScreen();
    printString("Exiting Pongis Golf...", screenWidth/2, screenHeight/2, 2, screenWidth, screenHeight, ALIGN_CENTER);
    sys_nano_sleep(40);
    clearScreen();
    flushKeyboardBuffer();
}

static void processPlayerMovements(Player *players, uint32_t width, uint32_t height) {
    for (int i = 0; i < numberOfPlayers; i++) {
        Player *player = &players[i];
        
        if (isKeyPressed(player->rotate_left)) {
            player->angle -= ROTATION_SPEED;
        }
        if (isKeyPressed(player->rotate_right)) {
            player->angle += ROTATION_SPEED;
        }
        
        while (player->angle < 0) player->angle += 2 * M_PI;
        while (player->angle >= 2 * M_PI) player->angle -= 2 * M_PI;
        
        if (isKeyPressed(player->forward)) {
            player->speed += PLAYER_ACCELERATION;
            if (player->speed > PLAYER_MAX_SPEED) {
                player->speed = PLAYER_MAX_SPEED;
            }
        } else if (isKeyPressed(player->backward)) {
            player->speed -= PLAYER_ACCELERATION;
            if (player->speed < -PLAYER_MAX_SPEED / 2) {
                player->speed = -PLAYER_MAX_SPEED / 2; // Retroceso más lento
            }
        } else {
            player->speed *= PLAYER_DECELERATION;
            if (my_fabs(player->speed) < 0.1f) {
                player->speed = 0.0f;
            }
        }
        
        float dx = my_cos(player->angle) * player->speed;
        float dy = my_sin(player->angle) * player->speed;
        
        float new_x = player->x + dx;
        float new_y = player->y + dy;
        
        if (new_x >= player->radius && new_x <= width - player->radius) {
            player->x = new_x;
        } else {
            player->speed *= -0.3f;
        }
        
        if (new_y >= player->radius && new_y <= height - player->radius) {
            player->y = new_y;
        } else {
            player->speed *= -0.3f; 
        }
    }
}

static void resolvePlayerCollisions(Player *players, uint32_t width, uint32_t height) {
    if (numberOfPlayers < 2) return;
    
    Player *player1 = &players[0];
    Player *player2 = &players[1];
    
    float dx = player2->x - player1->x;
    float dy = player2->y - player1->y;
    float distance_squared = dx*dx + dy*dy;
    float min_distance = player1->radius + player2->radius + 2.0f; // +2 para separación mínima
    
    // Si hay colisión (distancia menor que la suma de radios)
    if (distance_squared < min_distance * min_distance && distance_squared > 0.0f) {
        float distance = sqrtf(distance_squared);
        
        float nx = dx / distance;  // Normal x
        float ny = dy / distance;  // Normal y
        
        float overlap = min_distance - distance;
        float separation = overlap * 0.5f + 1.0f; // Cada jugador se mueve la mitad + 1 pixel extra
        
        // Mover jugadores en direcciones opuestas
        player1->x -= nx * separation;
        player1->y -= ny * separation;
        player2->x += nx * separation;
        player2->y += ny * separation;
        
        // Verificar que no salgan de los límites después de la separación
        // Player 1
        if (player1->x < player1->radius) player1->x = player1->radius;
        if (player1->x > width - player1->radius) player1->x = width - player1->radius;
        if (player1->y < player1->radius) player1->y = player1->radius;
        if (player1->y > height - player1->radius) player1->y = height - player1->radius;
        
        // Player 2
        if (player2->x < player2->radius) player2->x = player2->radius;
        if (player2->x > width - player2->radius) player2->x = width - player2->radius;
        if (player2->y < player2->radius) player2->y = player2->radius;
        if (player2->y > height - player2->radius) player2->y = height - player2->radius;
        
        player1->speed *= 0.7f;
        player2->speed *= 0.7f;
    }
}

static void draw_player(Player *player) {
    draw_ball((uint64_t)player->x, (uint64_t)player->y, (uint64_t)player->radius, player->color);
    float line_length = player->radius * 0.8f;
    for (int i = 0; i < (int)line_length; i += 2) {
        float point_x = player->x + my_cos(player->angle) * i;
        float point_y = player->y + my_sin(player->angle) * i;
        draw_ball((uint64_t)point_x, (uint64_t)point_y, 1, COLOR_BLACK); // Línea negra
    }
}


static int is_stopped(float vx, float vy) {
    const float STOP_THRESHOLD = 0.1f; // Velocidad mínima para considerar la pelota detenida
    return (my_fabs(vx) < STOP_THRESHOLD && my_fabs(vy) < STOP_THRESHOLD);
}

/*======= Mostrar info del nivel =======*/
// Multi-player
static void printCurrentLevelInfo(){
    printStringf(2, 2, 2, screenWidth, screenHeight, ALIGN_LEFT, "LEVEL %d", current_level + 1);
    printStringf(2,26, 2, screenWidth, screenHeight, ALIGN_LEFT, "Hole size: %d", config.hole_radius);
    printString("Press 'esc' to return to main menu", 2, 55, 1.5, screenWidth, screenHeight, ALIGN_LEFT);
}

// Single-player
static void printSinglePlayerInfo(int attempts, int max_attempts, int hits) {
    printStringf(2, 2, 2, screenWidth, screenHeight, ALIGN_LEFT, "LEVEL %d", current_level + 1);
    printStringf(2, 26, 2, screenWidth, screenHeight, ALIGN_LEFT, "Hole size: %d", config.hole_radius);
    
    char hit_str[20] = "";
    for (int i = 0; i < hits && hits <= MAX_HITS; i++) {
        hit_str[i] = '*';
    }
    hit_str[hits] = '\0'; 
    printStringf(2, 50, 2, screenWidth, screenHeight, ALIGN_LEFT, "Hits: %s", hit_str);

    printStringf(2, 74, 1, screenWidth, screenHeight, ALIGN_LEFT, "Attempts: %d/%d", attempts, max_attempts);
    printString("Press 'esc' to return to main menu", 1, 90, 1, screenWidth, screenHeight, ALIGN_LEFT);
}

/*======= Funciones matemáticas que necesitabamos =======*/
static float my_sin(float x) {
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x3 * x2;
    float x7 = x5 * x2;
    
    return x - (x3 / 6.0f) + (x5 / 120.0f) - (x7 / 5040.0f);
}

static float my_cos(float x) {
    return my_sin(x + M_PI / 2.0f);
}

static float my_fabs(float x) {
    return (x < 0.0f) ? -x : x;
}
/*===================================================*/

static int int_to_string(int value, char *buffer) {
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }
    
    int is_negative = 0;
    int length = 0;
    char temp[12];
    
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }
    
    while (value > 0) {
        temp[length++] = '0' + (value % 10);
        value /= 10;
    }
    
    int total_length = length + is_negative;
    
    // Copiar buffer en orden correcto
    if (is_negative) {
        buffer[0] = '-';
    }
    
    for (int i = 0; i < length; i++) {
        buffer[is_negative + i] = temp[length - 1 - i];
    }
    
    buffer[total_length] = '\0';
    return total_length;
}

// para formatear strings
static void simple_sprintf(char *buffer, const char *format, va_list args) {
    char *buf_ptr = buffer;
    const char *fmt_ptr = format;
    
    while (*fmt_ptr != '\0') {
        if (*fmt_ptr == '%') {
            fmt_ptr++;
            
            switch (*fmt_ptr) {
                case 'd': {
                    int value = va_arg(args, int);
                    buf_ptr += int_to_string(value, buf_ptr);
                    break;
                }
                case 's': {
                    char *str = va_arg(args, char*);
                    while (*str != '\0') {
                        *buf_ptr++ = *str++;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    *buf_ptr++ = c;
                    break;
                }
                case '%': {
                    *buf_ptr++ = '%';
                    break;
                }
                default:
                *buf_ptr++ = '%';
                *buf_ptr++ = *fmt_ptr;
                break;
            }
        } else {
            *buf_ptr++ = *fmt_ptr;
        }
        fmt_ptr++;
    }
    *buf_ptr = '\0';
}

void printStringf(uint64_t x, uint64_t y, uint64_t size, uint32_t width, uint32_t height, int alignment, const char *format, ...) {
    char buffer[256]; // Buffer para el string formateado
    va_list args;
    va_start(args, format);
    simple_sprintf(buffer, format, args);
    va_end(args);
    printString(buffer, x, y, size, width, height, alignment);
}