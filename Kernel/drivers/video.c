#include <video.h>
#include <font.h>

#define WIDTH VBE_info->width
#define HEIGHT VBE_info->height
      
//#define Y_OFFSET (FONT_HEIGHT * font_size)
#define X_OFFSET (FONT_WIDTH * font_size)

static Point current_point = {0,0};
static Char buffer[CHAR_ROWS_FONT_MIN * CHAR_COLS_FONT_MIN];

static VBEInfoPtr VBE_info = (VBEInfoPtr) 0x0000000000005C00;
extern uint8_t font_bitmap[4096];
static uint64_t index = 0;

static Color bg_color = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
static Color font_color = {COLOR_ON, COLOR_ON, COLOR_ON};
static Color error_color = {COLOR_ON, COLOR_OFF, COLOR_OFF};
static uint64_t font_size = DEFAULT_FONT_SIZE;

static void empty_screen(Color new_bg_color);
static void tab();
static void printLetter(Char letter);
static void nl();
static void bs();
static void printAgain();
//static void print();


static void empty_screen(Color new_bg_color){
    draw_rectangle(0, 0, WIDTH, HEIGHT, new_bg_color);
}

int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, Color color){
    if(x+width > WIDTH || y+height > HEIGHT ) return ERROR;
    for (uint64_t i = 0; i < width; i++) {
        for (uint64_t j = 0; j < height; j++) {
            draw_pixel( x + i, y + j, color);
        }
    }
    return OK;
}

int64_t get_screen_data(Screen * screen){
    (*screen).width = WIDTH;
    (*screen).height = HEIGHT;
    return OK;
}

 
int64_t write(const char *buffer, int64_t size) {
    uint64_t i = 0;
    while (i < (uint64_t)size && buffer[i]) {
        switch (buffer[i]) {
            case '\n': nl();  break;
            case '\b': bs();  break;
            case '\t': tab(); break;
            default:
                if (buffer[i] >= FIRST_ASCII && buffer[i] <= LAST_ASCII) {
                    Char c = { buffer[i], STDOUT };
                    buffer[index++] = c;
                    printLetter(c);
                }
                break;
        }
        i++;
    }
    return i;
}

static void nl(void) {
    // avanzar al inicio de la siguiente línea
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t pos = current_point.x / (FONT_WIDTH * font_size);
    for (uint64_t i = pos; i < chars_per_line; i++)
        buffer[index++] = (Char){' ', STDOUT};
    current_point.x = 0;
    current_point.y += FONT_HEIGHT * font_size;
    if (current_point.y + FONT_HEIGHT * font_size > HEIGHT)
        printAgain();
}

static void bs(void) {
    if (current_point.x == 0 && current_point.y == 0) return;
    if (current_point.x == 0) {
        current_point.y -= FONT_HEIGHT * font_size;
        current_point.x = WIDTH - FONT_WIDTH * font_size;
    }
    current_point.x -= FONT_WIDTH * font_size;
    // borrar área del carácter
    draw_rectangle(current_point.x, current_point.y,
                   FONT_WIDTH * font_size, FONT_HEIGHT * font_size, bg_color);
    if (index) {
        buffer[--index].c = 0;
        buffer[index].fd = STDOUT;
    }
}

static void tab(void) {
    for (int i = 0; i < 4; i++) {
        Char c = { ' ', STDOUT };
        buffer[index++] = c;
        printLetter(c);
    }
}


int64_t draw_font(uint64_t x, uint64_t y, uint8_t ch, Color color, uint64_t size) {
    if (ch < FIRST_ASCII || ch > LAST_ASCII) return ERROR;
    uint32_t letter_index = (ch - ' ') * FONT_HEIGHT;
    for (uint64_t row = 0; row < FONT_HEIGHT; row++) {
        for (uint64_t col = 0; col < FONT_WIDTH; col++) {
            if ((font_bitmap[letter_index + row] >> (7 - col)) & 1) {
                draw_rectangle(x + col * size, y + row * size, size, size, color);
            }
        }
    }
    return OK;
}

int64_t draw_pixel(uint64_t x, uint64_t y, Color color){
    if(x >= WIDTH || y>= HEIGHT ) return ERROR;
    uint8_t * vram = (uint8_t *) VBE_info->vram;
    uint64_t index = (x * ((VBE_info->bpp)/8)) + (y * VBE_info->pitch);
    vram[index] = color.b;
    vram[index+1] = color.g;
    vram[index+2] = color.r;
    return OK;
}


static void printLetter(Char letter) {
    if (current_point.x + FONT_WIDTH * font_size > WIDTH) {
        current_point.x = 0;
        current_point.y += FONT_HEIGHT * font_size;
    }
    if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
        printAgain();
    }
    Color col = (letter.fd == STDOUT ? font_color : error_color);
    draw_font(current_point.x, current_point.y, letter.c, col, font_size);
    current_point.x += FONT_WIDTH * font_size;
}

static void printAgain(void) {
    //muevo buffer hacia arriba media pantalla
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_to_scroll = HEIGHT / (2 * FONT_HEIGHT * font_size);
    uint64_t start = lines_to_scroll * chars_per_line;
    empty_screen(bg_color);
    uint64_t j = 0;
    for (uint64_t i = start; i < index; i++, j++) {
        printLetter(buffer[i]);
        buffer[j] = buffer[i];
    }
    index = j;
}

void set_font_color(Color color){
    font_color=color;
}