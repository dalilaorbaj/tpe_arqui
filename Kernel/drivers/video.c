#include <video.h>
#include <stddef.h>

// --- Información de video y configuración de pantalla ---
static VBEInfoPtr VBE_info = (VBEInfoPtr) 0x0000000000005C00;
#define WIDTH VBE_info->width
#define HEIGHT VBE_info->height
#define BPP VBE_info->bpp
#define PITCH VBE_info->pitch

static Point current_point = {0,0};
static const uint64_t MIN_FONT_SIZE = 1;
static const uint64_t MAX_FONT_SIZE = 5;
static const uint64_t FONT_SIZE_STEP = 1;

// --- Bitmaps de las 2 fonts ---
extern unsigned char default_font_bitmap[4096];
extern unsigned char dyslexic_font_bitmap[4096];
unsigned char * font_bitmap = default_font_bitmap;

static Color bg_color = {BLACK, BLACK, BLACK};
static uint64_t font_size = MIN_FONT_SIZE;
static Color saved_bg_color = {BLACK, BLACK,BLACK};
static uint8_t bg_color_saved = 0;

// --- Buffer de texto y cachés de layout ---
static Character raw_text[MAX_CHARS];
static uint64_t raw_text_length = 0;
static uint64_t cached_chars_per_line = 0;
static uint64_t cached_lines_per_screen = 0;
static uint64_t cached_font_size = 0;

static void updateCache();
static void append_char(char c, Color color);
static void simpleScroll();
static void rewriteFromRawText();
static void calculateCursorFromText();


void switch_to_dyslexic_font(void) {
    font_bitmap = dyslexic_font_bitmap;
    updateCache();
    empty_screen(bg_color);
    rewriteFromRawText();
}
void switch_to_default_font(void) {
    font_bitmap = default_font_bitmap;
    updateCache();
    empty_screen(bg_color);
    rewriteFromRawText();
}

/* Actualiza los valores cacheados de caracteres por línea y líneas por pantalla*/
static void updateCache() {
    if (font_size == 0){
        font_size = MIN_FONT_SIZE;
    } 
    if (WIDTH == 0 || HEIGHT == 0){
        return;
    } 
    if (cached_font_size != font_size) {
        cached_chars_per_line = WIDTH / (FONT_WIDTH * font_size);
        cached_lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
        cached_font_size = font_size;
        
        if (!cached_chars_per_line){
            cached_chars_per_line = 1;
        } 
        if (cached_lines_per_screen == 0){
            cached_lines_per_screen = 1;
        } 
    }
}

/**
 * Realiza un "scroll" de la pantalla moviendo el contenido hacia arriba
 * cuando se alcanza el final.
 */
static void simpleScroll() {
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    uint64_t scroll_pixels = FONT_HEIGHT * font_size;
    uint64_t bytes_per_row = PITCH;
    
    for (uint64_t y = 0; y < HEIGHT - scroll_pixels; y++) {
        uint64_t src_offset = (y + scroll_pixels) * bytes_per_row;
        uint64_t dst_offset = y * bytes_per_row;
        
        for (uint64_t x = 0; x < bytes_per_row; x += 8) {
            if (x + 8 <= bytes_per_row) {
                *((uint64_t*)(vram + dst_offset + x)) = *((uint64_t*)(vram + src_offset + x));
            } else {
                for (uint64_t i = x; i < bytes_per_row; i++) {
                    vram[dst_offset + i] = vram[src_offset + i];
                }
            }
        }
    }
    
    draw_rectangle(0, HEIGHT - scroll_pixels, WIDTH, scroll_pixels, bg_color);
    
    current_point.y -= FONT_HEIGHT * font_size;
    if (current_point.y < 0) current_point.y = 0;
}

/**
 * Agrega un carácter y su color al buffer de texto crudo.
 * Si el buffer está lleno, descarta los primeros caracteres.
 */
static void append_char(char c, Color color) {
    if (raw_text_length >= MAX_CHARS - 1) {
        uint64_t shift = MAX_CHARS / 4;
        for (uint64_t i = 0; i < raw_text_length - shift; i++) {
            raw_text[i] = raw_text[i + shift];
        }
        raw_text_length -= shift;
    }
    
    if (raw_text_length < MAX_CHARS) {
        raw_text[raw_text_length++] = (Character){c, color};
    }
}

/**
 * Calcula la posición del cursor en pantalla a partir del buffer de texto crudo.
 */
static void calculateCursorFromText() {
    if (raw_text_length == 0) {
        current_point.x = 0;
        current_point.y = 0;
        return;
    }
    
    updateCache();
    
    uint64_t total_lines = 0;
    uint64_t line_chars = 0;
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            total_lines++;
            line_chars = 0;
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            if (line_chars >= cached_chars_per_line) {
                total_lines++;
                line_chars = 0;
            }
            line_chars++;
        }
    }
    
    if (line_chars > 0) {
        total_lines++;
    }
    
    uint64_t start_line = 0;
    if (total_lines > cached_lines_per_screen) {
        start_line = total_lines - cached_lines_per_screen;
    }
    
    uint64_t current_line = 0;
    uint64_t cursor_x = 0, cursor_y = 0;
    line_chars = 0;
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            current_line++;
            line_chars = 0;
            
            if (current_line >= start_line) {
                cursor_x = 0;
                cursor_y += FONT_HEIGHT * font_size;
            }
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            if (line_chars >= cached_chars_per_line) {
                current_line++;
                line_chars = 0;
                
                if (current_line >= start_line) {
                    cursor_x = 0;
                    cursor_y += FONT_HEIGHT * font_size;
                }
            }
            
            if (current_line >= start_line) {
                cursor_x += FONT_WIDTH * font_size;
            }
            line_chars++;
        }
    }
    
    current_point.x = cursor_x;
    current_point.y = cursor_y;
    
    if (current_point.y < 0) current_point.y = 0;
    if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
        current_point.y = HEIGHT - FONT_HEIGHT * font_size;
    }
}

/**
 * Redibuja todo el texto almacenado en el buffer crudo sobre la pantalla.
 */
static void rewriteFromRawText() {
    if (raw_text_length == 0) {
        current_point.x = 0;
        current_point.y = 0;
        return;
    }
    
    updateCache();
    
    uint64_t total_lines = 0;
    uint64_t line_chars = 0;
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            total_lines++;
            line_chars = 0;
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            if (line_chars >= cached_chars_per_line) {
                total_lines++;
                line_chars = 0;
            }
            line_chars++;
        }
    }
    
    if (line_chars > 0) {
        total_lines++;
    }
    
    uint64_t start_line = 0;
    if (total_lines > cached_lines_per_screen) {
        start_line = total_lines - cached_lines_per_screen;
    }
    
    uint64_t current_line = 0;
    uint64_t x = 0, y = 0;
    line_chars = 0;
    uint8_t should_render = (start_line == 0);
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            current_line++;
            line_chars = 0;
            
            if (current_line >= start_line && !should_render) {
                should_render = 1;
                x = 0;
                y = 0;
            } else if (should_render) {
                x = 0;
                y += FONT_HEIGHT * font_size;
                
                if (y + FONT_HEIGHT * font_size > HEIGHT) {
                    break;
                }
            }
            
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            if (line_chars >= cached_chars_per_line) {
                current_line++;
                line_chars = 0;
                
                if (current_line >= start_line && !should_render) {
                    should_render = 1;
                    x = 0;
                    y = 0;
                } else if (should_render) {
                    x = 0;
                    y += FONT_HEIGHT * font_size;
                    
                    if (y + FONT_HEIGHT * font_size > HEIGHT) {
                        break;
                    }
                }
            }
            
            if (should_render && y + FONT_HEIGHT * font_size <= HEIGHT) {
                draw_font(x, y, raw_text[i].c, raw_text[i].color, font_size);
                x += FONT_WIDTH * font_size;
            }
            
            line_chars++;
        }
    }
    
    calculateCursorFromText();
}


void empty_screen(Color new_bg_color){
    bg_color = new_bg_color;
    draw_rectangle(0, 0, WIDTH, HEIGHT, new_bg_color);
    current_point.x = 0;
    current_point.y = 0;
}

int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, Color color){
    if(x+width > WIDTH || y+height > HEIGHT ) return ERROR;
    
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    
    for (uint64_t j = 0; j < height; j++) {
        uint64_t row_start = ((y + j) * PITCH) + (x * (BPP/8));
        for (uint64_t i = 0; i < width; i++) {
            uint64_t index = row_start + (i * (BPP/8));
            vram[index] = color.blue;
            vram[index+1] = color.green;
            vram[index+2] = color.red;
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

/**
 * Borra todos los buffers de texto y limpia la pantalla.
 */
void clear_video_buffers(void) {
    raw_text_length = 0;   
    current_point.x = 0;
    current_point.y = 0;
    
    empty_screen(bg_color);
    
    for (uint64_t i = 0; i < MAX_CHARS; i++) {
        raw_text[i].c = 0;
        raw_text[i].color = (Color){0, 0, 0};
    }
}

uint64_t write(const char * buf, int64_t size, Color color) {
    static uint8_t first_call = 1;
    
    if (first_call) {
        updateCache();
        first_call = 0;
    }
    
    if (!buf || size <= 0) return 0;
    
    updateCache();
    uint64_t i = 0;
    
    while (i < (uint64_t)size && buf[i]) {
        switch (buf[i]) {
            case '\n': 
                append_char('\n', color);
                current_point.x = 0;
                current_point.y += FONT_HEIGHT * font_size;
                
                if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                    simpleScroll();
                }
                break;
                
            case '\b': 
                if (raw_text_length > 0 && current_point.x >= FONT_WIDTH * font_size) {
                    current_point.x -= FONT_WIDTH * font_size;
                    raw_text_length--;
                    // Borrar carácter dibujando un rectángulo del color de fondo
                    draw_rectangle(current_point.x, current_point.y, FONT_WIDTH * font_size, FONT_HEIGHT * font_size, bg_color);
                }
                break;
                
            case '\t': 
                for(int j = 0; j < 4; j++) {
                    if (current_point.x + FONT_WIDTH * font_size <= WIDTH) {
                        append_char(' ', color);
                        draw_font(current_point.x, current_point.y, ' ', color, font_size);
                        current_point.x += FONT_WIDTH * font_size;
                    } else {
                        current_point.x = 0;
                        current_point.y += FONT_HEIGHT * font_size;
                        if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                            simpleScroll();
                        }
                    }
                }
                break;
                
            default:
                if (buf[i] >= FIRST_ASCII && buf[i] <= LAST_ASCII) {
                    append_char(buf[i], color);
                    
                    draw_font(current_point.x, current_point.y, buf[i], color, font_size);
                    
                    current_point.x += FONT_WIDTH * font_size;
                    
                    if (current_point.x + FONT_WIDTH * font_size > WIDTH) {
                        current_point.x = 0;
                        current_point.y += FONT_HEIGHT * font_size;
                        
                        if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                            simpleScroll();
                        }
                    }
                }
                break;
        }
        i++;
    }
    return i;
}

int64_t draw_font(uint64_t x, uint64_t y, uint8_t ch, Color color, uint64_t size) {
    if (ch < FIRST_ASCII || ch > LAST_ASCII) return ERROR;
    unsigned char * glyph = fontPixelMap(ch, font_bitmap);
    for (uint64_t row = 0; row < FONT_HEIGHT; row++) {
        unsigned char bits = glyph[row];
        for (uint64_t col = 0; col < FONT_WIDTH; col++) {
            if ((bits >> (7 - col)) & 1) {
                draw_rectangle(x + col * size, y + row * size, size, size, color);
            }
        }
    }
    return OK;
}

int64_t draw_pixel(uint64_t x, uint64_t y, Color color){
    if(x >= WIDTH || y>= HEIGHT ) return ERROR;
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    uint64_t index = (y * PITCH) + (x * (BPP/8));
    vram[index++] = color.blue;
    vram[index++] = color.green;
    vram[index] = color.red;
    return OK;
}

void increaseFontSize(void) {
    if (font_size < MAX_FONT_SIZE) {
        font_size += FONT_SIZE_STEP;
        updateCache();
        empty_screen(bg_color);
        rewriteFromRawText();
    }
}

void decreaseFontSize(void) {
    if (font_size > MIN_FONT_SIZE) {
        font_size -= FONT_SIZE_STEP;
        updateCache();
        empty_screen(bg_color);
        rewriteFromRawText();
    }
}

/**
 * Restaura el tamaño de fuente y el color de fondo a los valores iniciales,
 * y limpia la pantalla.
 */
void resetScreen(Color newBgColor) {
    if (!bg_color_saved && newBgColor.red == 139 && newBgColor.green == 0 && newBgColor.blue == 36) {
        saved_bg_color = bg_color;
        bg_color_saved = 1;
    }
    
    font_size = MIN_FONT_SIZE;
    updateCache();
    clear_video_buffers();
    empty_screen(newBgColor);
}