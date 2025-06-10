#include <pongisGolf.h>

#define SHELL_BUFFER_SIZE 100
#define COMMAND_MAX_LENGTH 20
#define ARGS_MAX_LENGTH 90
#define CANT_OPTIONS 16
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/* =====Colores utilizados en la Shell===== */
#define COLOR_ORANGE (Color){129, 201, 255}
#define COLOR_YELLOW (Color){129, 243, 255}
#define COLOR_MINT (Color){158, 255, 220}
#define COLOR_PURPLE (Color){255, 158, 161}

typedef struct Option{
    char * name;
    void (* function)();
} Option;

