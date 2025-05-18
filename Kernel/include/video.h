#include <stdint.h>

#define DEFAULT_FONT_SIZE 1
#define COLOR_OFF 0
#define COLOR_ON 255
#define TEXT_MODE 0
#define VIDEO_MODE 1

#define ERROR -1
#define EARLY_EXIT 0
#define OK 1

typedef struct{
    uint64_t x;
    uint64_t y;
}Point;

typedef struct{
    uint8_t c;
    uint8_t fd;
}Char;

typedef struct{
    int64_t width;
    int64_t height;
} Screen;