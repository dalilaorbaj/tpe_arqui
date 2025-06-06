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

// Variable para controlar cuándo hacer redraw completo
static uint8_t needs_full_redraw = 0;

// Prototipos de funciones
static void rewriteFromRawText();
static void append_with_fd(char c, Color color);
static void smartScroll();

static uint64_t get_max_chars() {
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
    return chars_per_line * lines_per_screen;
}

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

// Función para sincronizar el estado del video
void sync_video_state(void) {
    // Asegurar que current_point esté alineado con el contenido actual
    if (raw_text_length == 0) {
        current_point.x = 0;
        current_point.y = 0;
        return;
    }
    
    // Recalcular current_point basado en raw_text
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t x = 0, y = 0;
    uint64_t current_line_chars = 0;
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            x = 0;
            y += FONT_HEIGHT * font_size;
            current_line_chars = 0;
        } else {
            if (current_line_chars >= chars_per_line) {
                x = 0;
                y += FONT_HEIGHT * font_size;
                current_line_chars = 0;
            }
            x += FONT_WIDTH * font_size;
            current_line_chars++;
        }
    }
    
    current_point.x = x;
    current_point.y = y;
}

// Función para limpiar completamente todos los buffers
void clear_video_buffers(void) {
    // Limpiar todos los buffers
    raw_text_length = 0;
    char_index = 0;
    
    // Resetear posición del cursor
    current_point.x = 0;
    current_point.y = 0;
    
    // Limpiar pantalla
    empty_screen(bg_color);
    
    // Limpiar buffer visual
    for (uint64_t i = 0; i < MAX_CHARS; i++) {
        buffer[i].c = 0;
        buffer[i].color = (Color){0, 0, 0};
    }
    
    // Limpiar raw_text
    for (uint64_t i = 0; i < MAX_CHARS; i++) {
        raw_text[i].c = 0;
        raw_text[i].color = (Color){0, 0, 0};
    }
    
    // Resetear flag de primera escritura para forzar sincronización
    needs_full_redraw = 0;
}

uint64_t write(const char * buf, int64_t size, Color color) {
    static uint8_t first_write = 1;
    
    // En la primera escritura, sincronizar el estado
    if (first_write) {
        sync_video_state();
        first_write = 0;
    }
    
    uint64_t i = 0;
    uint8_t needs_redraw = 0;
    
    while (i < (uint64_t)size && buf[i]) {
        switch (buf[i]) {
            case '\n': 
                append_with_fd('\n', color);
                // Para \n, solo mover cursor sin redraw completo
                current_point.x = 0;
                current_point.y += FONT_HEIGHT * font_size;
                
                // Solo redraw si se sale de pantalla
                if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                    needs_redraw = 1;
                }
                break;
            case '\b': 
                // Manejo de backspace: borrar el área del último carácter
                if (raw_text_length > 0) {
                    // Calcular posición del carácter a borrar
                    uint64_t back_x = current_point.x - FONT_WIDTH * font_size;
                    if (current_point.x == 0 && current_point.y > 0) {
                        // Si estamos al inicio de línea, ir al final de la anterior
                        current_point.y -= FONT_HEIGHT * font_size;
                        uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
                        current_point.x = (chars_per_line - 1) * FONT_WIDTH * font_size;
                        back_x = current_point.x;
                    } else {
                        current_point.x = back_x;
                    }
                    
                    // Borrar solo el área del carácter
                    draw_rectangle(back_x, current_point.y, 
                                 FONT_WIDTH * font_size, FONT_HEIGHT * font_size, bg_color);
                    
                    raw_text_length--;
                }
                break;
            case '\t': 
                // 4 espacios para tab
                for(int j = 0; j < 4; j++) {
                    append_with_fd(' ', color);
                    
                    // Dibujar cada espacio directamente
                    draw_font(current_point.x, current_point.y, ' ', color, font_size);
                    current_point.x += FONT_WIDTH * font_size;
                    
                    // Verificar wrap
                    if (current_point.x + FONT_WIDTH * font_size > WIDTH) {
                        current_point.x = 0;
                        current_point.y += FONT_HEIGHT * font_size;
                        if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                            needs_redraw = 1;
                            break;
                        }
                    }
                }
                break;
            default:
                if (buf[i] >= FIRST_ASCII && buf[i] <= LAST_ASCII) {
                    append_with_fd(buf[i], color);
                    
                    // DIBUJO DIRECTO sin redraw completo
                    Character last_char = raw_text[raw_text_length - 1];
                    draw_font(current_point.x, current_point.y, last_char.c, last_char.color, font_size);
                    current_point.x += FONT_WIDTH * font_size;
                    
                    // Verificar si necesitamos wrap de línea
                    if (current_point.x + FONT_WIDTH * font_size > WIDTH) {
                        current_point.x = 0;
                        current_point.y += FONT_HEIGHT * font_size;
                        
                        // Solo si se sale de pantalla hacer redraw completo
                        if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
                            needs_redraw = 1;
                        }
                    }
                }
                break;
        }
        i++;
    }
    
    // Solo hacer redraw completo cuando sea absolutamente necesario
    if (needs_redraw || needs_full_redraw) {
        rewriteFromRawText();
        needs_full_redraw = 0;
    }
    
    return i;
}

// Función para scroll inteligente sin borrar toda la pantalla
static void smartScroll() {
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
    uint64_t scroll_lines = lines_per_screen / 2; // Scroll media pantalla
    
    // Copiar visualmente las líneas de abajo hacia arriba (simulando scroll)
    uint64_t scroll_pixels = scroll_lines * FONT_HEIGHT * font_size;
    
    // Usar memmove para copiar los píxeles hacia arriba
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    uint64_t bytes_per_line = WIDTH * (BPP / 8);
    
    for (uint64_t y = 0; y < HEIGHT - scroll_pixels; y++) {
        uint64_t src_offset = (y + scroll_pixels) * PITCH;
        uint64_t dst_offset = y * PITCH;
        for (uint64_t x = 0; x < bytes_per_line; x++) {
            vram[dst_offset + x] = vram[src_offset + x];
        }
    }
    
    // Limpiar solo la parte inferior
    draw_rectangle(0, HEIGHT - scroll_pixels, WIDTH, scroll_pixels, bg_color);
    
    // Ajustar current_point
    current_point.y -= scroll_pixels;
    if (current_point.y < 0) current_point.y = 0;
}

static void rewriteFromRawText() {
    // Verificar si podemos hacer scroll inteligente en lugar de clear completo
    if (!needs_full_redraw && current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
        smartScroll();
        return;
    }
    
    // Calcular cuántos caracteres caben por línea con el font_size actual
    uint64_t chars_per_line = WIDTH / (FONT_WIDTH * font_size);
    uint64_t lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
    
    // Calcular start basado en LÍNEAS, no caracteres
    uint64_t start = 0;
    uint64_t line_count = 0;
    uint64_t char_in_current_line = 0;
    
    // Recorrer desde el final del buffer hacia atrás
    for (int64_t i = raw_text_length - 1; i >= 0; i--) {
        if (raw_text[i].c == '\n') {
            line_count++;
            char_in_current_line = 0;
            if (line_count >= lines_per_screen) {
                start = i + 1;
                break;
            }
        } else {
            char_in_current_line++;
            if (char_in_current_line > chars_per_line) {
                line_count++;
                char_in_current_line = 1;
                if (line_count >= lines_per_screen) {
                    start = i;
                    break;
                }
            }
        }
    }
    
    // Solo limpiar la pantalla cuando sea absolutamente necesario
    if (needs_full_redraw) {
        empty_screen(bg_color);
    }
    
    // Resetear posición
    uint64_t x = 0, y = 0;
    uint64_t current_line_chars = 0;
    
    // Recorrer raw_text desde start y redibujar
    for (uint64_t i = start; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            x = 0;
            y += FONT_HEIGHT * font_size;
            current_line_chars = 0;
            
            if (y + FONT_HEIGHT * font_size > HEIGHT) {
                break;
            }
        } else {
            if (current_line_chars >= chars_per_line) {
                x = 0;
                y += FONT_HEIGHT * font_size;
                current_line_chars = 0;
                
                if (y + FONT_HEIGHT * font_size > HEIGHT) {
                    break;
                }
            }
            
            draw_font(x, y, raw_text[i].c, raw_text[i].color, font_size);
            
            x += FONT_WIDTH * font_size;
            current_line_chars++;
        }
    }
    
    // Actualizar current_point para próximas escrituras
    current_point.x = x;
    current_point.y = y;
    
    // Sincronizar buffer visual
    char_index = 0;
    uint64_t buf_x = 0, buf_y = 0, buf_line_chars = 0;
    for (uint64_t i = start; i < raw_text_length && char_index < MAX_CHARS; i++) {
        if (raw_text[i].c == '\n') {
            buf_x = 0;
            buf_y += FONT_HEIGHT * font_size;
            buf_line_chars = 0;
        } else {
            if (buf_line_chars >= chars_per_line) {
                buf_x = 0;
                buf_y += FONT_HEIGHT * font_size;
                buf_line_chars = 0;
            }
            if (char_index < MAX_CHARS) {
                buffer[char_index++] = raw_text[i];
            }
            buf_line_chars++;
        }
        if (buf_y + FONT_HEIGHT * font_size > HEIGHT) break;
    }
}

int64_t draw_font(uint64_t x, uint64_t y, uint8_t ch, Color color, uint64_t size) {
    if (ch < FIRST_ASCII || ch > LAST_ASCII) return ERROR;

    unsigned char *glyph = fontPixelMap(ch);
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
    uint64_t index = (x * ((VBE_info->bpp)/8)) + (y * VBE_info->pitch);
    vram[index] = color.blue;
    vram[index+1] = color.green;
    vram[index+2] = color.red;
    return OK;
}

void _color(Color color){
    font_color = color;
}

static void append_with_fd(char c, Color color) {
    // Si el buffer está lleno, eliminar los primeros caracteres (scroll)
    if (raw_text_length >= MAX_CHARS - 1) {
        uint64_t shift = MAX_CHARS / 4; // Eliminar 1/4 del buffer
        for (uint64_t i = shift; i < raw_text_length; i++) {
            raw_text[i - shift] = raw_text[i];
        }
        raw_text_length -= shift;
        needs_full_redraw = 1; // Marcar que se necesita redraw completo después del scroll
    }
    
    raw_text[raw_text_length++] = (Character){c, color};
}

void increaseFontSize(void) {
    if (font_size < MAX_FONT_SIZE) {
        font_size += FONT_SIZE_STEP;
        needs_full_redraw = 1;
        rewriteFromRawText();
    }
}

void decreaseFontSize(void) {
    if (font_size > MIN_FONT_SIZE) {
        font_size -= FONT_SIZE_STEP;
        needs_full_redraw = 1;
        rewriteFromRawText();
    }
}