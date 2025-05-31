#include <video.h>
#include <font.h>
#include <stddef.h>

//Estructura que contiene información sobre el modo de video (resolución, bits por píxel, etc.)
static VBEInfoPtr VBE_info = (VBEInfoPtr) 0x0000000000005C00;

#define WIDTH VBE_info->width
#define HEIGHT VBE_info->height
#define BPP VBE_info->bpp
#define PITCH VBE_info->pitch
      
#define CHAR_ROWS_FONT_MIN 25
#define CHAR_COLS_FONT_MIN 80
#define FIRST_ASCII 32
#define LAST_ASCII 126

#define MAX_CHARS 8000 

//#define Y_OFFSET (FONT_HEIGHT * font_size)
#define X_OFFSET (FONT_WIDTH * font_size)

//Estructura para mantener la posición actual del cursor
static Point current_point = {0,0};
static Character buffer[MAX_CHARS];

extern unsigned char font_bitmap[4096];
static uint64_t char_index = 0;

static Color bg_color = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
static Color font_color = {COLOR_ON, COLOR_ON, COLOR_ON};
static Color error_color = {COLOR_ON, COLOR_OFF, COLOR_OFF};
static uint64_t font_size = MIN_FONT_SIZE;

static Character raw_text[MAX_CHARS];
static uint64_t raw_text_length = 0;

static uint64_t get_max_chars() {
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
    return chars_per_line * lines_per_screen;
}

static void tab();
static void draw_letter(Character letter);
static void nl();
static void bs();
static void printAgain();
//static void print();
static void append_with_fd(char c, Color color);
static void append(char c);


void empty_screen(Color new_bg_color){
    draw_rectangle(0, 0, WIDTH, HEIGHT, new_bg_color);
    current_point.x = 0;
    current_point.y = 0;
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

int64_t get_screen_info(Screen * screen){
    if(screen == NULL) {
        return ERROR;
    }
    screen->width = WIDTH;
    screen->height = HEIGHT;
    screen->bpp = BPP;
    screen->pitch = PITCH;
    return OK;
}

 
uint64_t write(const char * buf, int64_t size, Color color) {
    uint64_t i = 0;
    while (i < (uint64_t)size && buf[i]) {
        switch (buf[i]) {
            case '\n': append_with_fd('\n', color); nl();  break;
            case '\b': bs();  break;
            case '\t': tab(); break;
            default:
                if (buf[i] >= FIRST_ASCII && buf[i] <= LAST_ASCII) {
                    set_font_color(color);
                    append_with_fd(buf[i], color);
                    draw_letter((Character){buf[i], color});
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
        buffer[char_index++] = (Character){' ', STDOUT};
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
    if (char_index) {
        buffer[--char_index].c = 0;
        buffer[char_index].color = font_color;
    }
}

static void tab(void) {
    for (int i = 0; i < 4; i++) {
        Character c = { ' ', font_color };
        buffer[char_index++] = c;
        draw_letter(c);
    }
}


int64_t draw_font(uint64_t x, uint64_t y, uint8_t ch, Color color, uint64_t size) {
    if (ch < FIRST_ASCII || ch > LAST_ASCII) return ERROR; // chequeamos que sea un caracter imprimible

    unsigned char *glyph = fontPixelMap(ch); // buscamos al caracter en el bitmap
    for (uint64_t row = 0; row < FONT_HEIGHT; row++) { // itera sobre las "filas" de pixeles que ocupa el caracter
        unsigned char bits = glyph[row];
        for (uint64_t col = 0; col < FONT_WIDTH; col++) { // itera sobre las "columnas" de pixeles que ocupa el caracter
            if ((bits >> (7 - col)) & 1) { // agarramos el bit correspondiente a la columna
                draw_rectangle(x + col * size, y + row * size, size, size, color);
            }
        }
    }
    return OK;
}

int64_t draw_pixel(uint64_t x, uint64_t y, Color color){
    if(x >= WIDTH || y>= HEIGHT ) return ERROR;
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    uint64_t index = (x * ((VBE_info->bpp)/8)) + (y * VBE_info->pitch);
    vram[index] = color.blue;
    vram[index+1] = color.green;
    vram[index+2] = color.red;
    return OK;
}

static void draw_letter(Character letter) {
    draw_font(current_point.x, current_point.y, letter.c, letter.color, font_size);
    current_point.x += FONT_WIDTH * font_size;
}

static void printAgain(void) {
    //muevo buffer hacia arriba media pantalla
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_to_scroll = HEIGHT / (2 * FONT_HEIGHT * font_size);
    uint64_t start = lines_to_scroll * chars_per_line;
    empty_screen(bg_color);
    uint64_t j = 0;
    for (uint64_t i = start; i < char_index; i++, j++) {
        draw_letter(buffer[i]);
        buffer[j] = buffer[i];
    }
    char_index = j;
}

void set_font_color(Color color){
    font_color=color;
}

static void append_with_fd(char c, Color color) {
    if (char_index >= MAX_CHARS) {
        printAgain();
    }
    buffer[char_index].c = c;
    buffer[char_index].color = color;
    char_index++;

    if (raw_text_length < MAX_CHARS - 1) {
        raw_text[raw_text_length++] = (Character){c, color};
    }
}

static void append(char c) {
    append_with_fd(c, font_color); // Usar color por defecto
}


static void rewriteFromRawText() {
    Character temp_text[MAX_CHARS];
    uint64_t temp_length = raw_text_length;

    for (uint64_t i = 0; i < temp_length; i++) {
        temp_text[i] = raw_text[i];
    }

    // Limpiar solo el buffer y la posición
    char_index = 0;
    current_point.x = 0;
    current_point.y = 0;

    for (uint64_t i = 0; i < temp_length; i++) {
    if (temp_text[i].c == '\n') {
        current_point.x = 0;
        current_point.y += FONT_HEIGHT * font_size;
    } else {
        buffer[char_index].c = temp_text[i].c;
        buffer[char_index].color = temp_text[i].color;
        char_index++;
        draw_letter(temp_text[i]); 
    }
}
}

// ✅ Simplificar funciones de zoom
void increaseFontSize(void) {
    if (font_size < MAX_FONT_SIZE) {
        font_size += FONT_SIZE_STEP;
        empty_screen(bg_color);
        rewriteFromRawText();  
    }
}

void decreaseFontSize(void) {
    if (font_size > MIN_FONT_SIZE) {
        font_size -= FONT_SIZE_STEP;
        empty_screen(bg_color);
        rewriteFromRawText();
    }
}