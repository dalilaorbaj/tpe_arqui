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

// Variables para optimización - SIMPLIFICADAS
static Character raw_text[MAX_CHARS];
static uint64_t raw_text_length = 0;
static uint64_t cached_chars_per_line = 0;
static uint64_t cached_lines_per_screen = 0;
static uint64_t cached_font_size = 0;

// Variable para guardar el color de fondo original antes de excepciones
static Color saved_bg_color = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
static uint8_t bg_color_saved = 0;

// Prototipos de funciones internas
static void updateCache();
static void append_char(char c, Color color);
static void simpleScroll();
static void rewriteFromRawText();
static void calculateCursorFromText();

// Actualizar cache de cálculos
static void updateCache() {
    if (font_size == 0) font_size = MIN_FONT_SIZE;
    if (WIDTH == 0 || HEIGHT == 0) return;
    
    if (cached_font_size != font_size) {
        cached_chars_per_line = WIDTH / (FONT_WIDTH * font_size);
        cached_lines_per_screen = HEIGHT / (FONT_HEIGHT * font_size);
        cached_font_size = font_size;
        
        if (cached_chars_per_line == 0) cached_chars_per_line = 1;
        if (cached_lines_per_screen == 0) cached_lines_per_screen = 1;
    }
}

// Scroll simple y rápido
static void simpleScroll() {
    uint8_t * vram = (uint8_t *)(uint64_t)VBE_info->framebuffer;
    uint64_t scroll_pixels = FONT_HEIGHT * font_size;
    uint64_t bytes_per_row = PITCH;
    
    // Mover contenido hacia arriba usando memmove-like operation
    for (uint64_t y = 0; y < HEIGHT - scroll_pixels; y++) {
        uint64_t src_offset = (y + scroll_pixels) * bytes_per_row;
        uint64_t dst_offset = y * bytes_per_row;
        
        // Copiar fila completa de una vez
        for (uint64_t x = 0; x < bytes_per_row; x += 8) {
            if (x + 8 <= bytes_per_row) {
                *((uint64_t*)(vram + dst_offset + x)) = *((uint64_t*)(vram + src_offset + x));
            } else {
                // Copiar bytes restantes
                for (uint64_t i = x; i < bytes_per_row; i++) {
                    vram[dst_offset + i] = vram[src_offset + i];
                }
            }
        }
    }
    
    // Limpiar área inferior
    draw_rectangle(0, HEIGHT - scroll_pixels, WIDTH, scroll_pixels, bg_color);
    
    current_point.y -= FONT_HEIGHT * font_size;
    if (current_point.y < 0) current_point.y = 0;
}

// Añadir carácter al buffer interno
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

// Calcular posición del cursor basándose en todo el texto - VERSIÓN UNIFICADA
static void calculateCursorFromText() {
    if (raw_text_length == 0) {
        current_point.x = 0;
        current_point.y = 0;
        return;
    }
    
    updateCache();
    
    // PASO 1: Simular todo el texto para contar líneas totales
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
    
    // Si hay caracteres en la última línea, contarla
    if (line_chars > 0) {
        total_lines++;
    }
    
    // PASO 2: Determinar línea de inicio para mostrar las últimas líneas
    uint64_t start_line = 0;
    if (total_lines > cached_lines_per_screen) {
        start_line = total_lines - cached_lines_per_screen;
    }
    
    // PASO 3: Calcular posición del cursor dentro de la ventana visible
    uint64_t current_line = 0;
    uint64_t cursor_x = 0, cursor_y = 0;
    line_chars = 0;
    
    for (uint64_t i = 0; i < raw_text_length; i++) {
        if (raw_text[i].c == '\n') {
            current_line++;
            line_chars = 0;
            
            // Solo actualizar cursor_y si estamos en la ventana visible
            if (current_line >= start_line) {
                cursor_x = 0;
                cursor_y += FONT_HEIGHT * font_size;
            }
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            // Check wrap por ancho
            if (line_chars >= cached_chars_per_line) {
                current_line++;
                line_chars = 0;
                
                if (current_line >= start_line) {
                    cursor_x = 0;
                    cursor_y += FONT_HEIGHT * font_size;
                }
            }
            
            // Solo actualizar cursor_x si estamos en la ventana visible
            if (current_line >= start_line) {
                cursor_x += FONT_WIDTH * font_size;
            }
            line_chars++;
        }
    }
    
    // PASO 4: Establecer posición final del cursor
    current_point.x = cursor_x;
    current_point.y = cursor_y;
    
    // PASO 5: Ajustar si el cursor se sale de la pantalla
    if (current_point.y < 0) current_point.y = 0;
    if (current_point.y + FONT_HEIGHT * font_size > HEIGHT) {
        current_point.y = HEIGHT - FONT_HEIGHT * font_size;
    }
}

// Re-renderizar todo el texto usando la MISMA lógica que calculateCursorFromText
static void rewriteFromRawText() {
    if (raw_text_length == 0) {
        current_point.x = 0;
        current_point.y = 0;
        return;
    }
    
    updateCache();
    
    // PASO 1: Contar líneas totales (MISMA lógica que calculateCursorFromText)
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
    
    // PASO 2: Determinar desde qué línea empezar (MISMA lógica)
    uint64_t start_line = 0;
    if (total_lines > cached_lines_per_screen) {
        start_line = total_lines - cached_lines_per_screen;
    }
    
    // PASO 3: Renderizar desde start_line (MISMA lógica de iteración)
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
                
                // Si nos salimos de pantalla, parar
                if (y + FONT_HEIGHT * font_size > HEIGHT) {
                    break;
                }
            }
            
        } else if (raw_text[i].c >= FIRST_ASCII && raw_text[i].c <= LAST_ASCII) {
            // Check wrap por ancho
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
                    
                    // Si nos salimos de pantalla, parar
                    if (y + FONT_HEIGHT * font_size > HEIGHT) {
                        break;
                    }
                }
            }
            
            // Renderizar carácter si debe ser visible
            if (should_render && y + FONT_HEIGHT * font_size <= HEIGHT) {
                draw_font(x, y, raw_text[i].c, raw_text[i].color, font_size);
                x += FONT_WIDTH * font_size;
            }
            
            line_chars++;
        }
    }
    
    // PASO 4: Calcular cursor usando la misma lógica
    calculateCursorFromText();
}

// === FUNCIONES PÚBLICAS (NO MODIFICAR PROTOTIPOS) ===

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

void clear_video_buffers(void) {
    raw_text_length = 0;
    char_index = 0;
    
    current_point.x = 0;
    current_point.y = 0;
    
    empty_screen(bg_color);
    
    for (uint64_t i = 0; i < MAX_CHARS; i++) {
        buffer[i].c = 0;
        buffer[i].color = (Color){0, 0, 0};
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
    
    // Procesar caracteres UNO A UNO sin batch rendering
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
                    draw_rectangle(current_point.x, current_point.y, 
                                 FONT_WIDTH * font_size, FONT_HEIGHT * font_size, bg_color);
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
                    
                    // Dibujar carácter INMEDIATAMENTE
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
    uint64_t index = (y * PITCH) + (x * (BPP/8));
    vram[index] = color.blue;
    vram[index+1] = color.green;
    vram[index+2] = color.red;
    
    return OK;
}

void _color(Color color){
    font_color = color;
}

void increaseFontSize(void) {
    if (font_size < MAX_FONT_SIZE) {
        font_size += FONT_SIZE_STEP;
        updateCache();
        
        // Limpiar pantalla y re-renderizar garantizando últimas líneas visibles
        empty_screen(bg_color);
        rewriteFromRawText();
    }
}

void decreaseFontSize(void) {
    if (font_size > MIN_FONT_SIZE) {
        font_size -= FONT_SIZE_STEP;
        updateCache();
        
        // Limpiar pantalla y re-renderizar garantizando últimas líneas visibles
        empty_screen(bg_color);
        rewriteFromRawText();
    }
}

void resetScreen(Color newBgColor) {
    // Guardar el color de fondo anterior solo si no es de excepción
    if (!bg_color_saved && newBgColor.red == 139 && newBgColor.green == 0 && newBgColor.blue == 36) {
        // Se está entrando en modo excepción, guardar el color actual
        saved_bg_color = bg_color;
        bg_color_saved = 1;
    }
    
    font_size = MIN_FONT_SIZE;
    updateCache();
    clear_video_buffers();
    empty_screen(newBgColor);
}

void sync_video_state(void) {
    calculateCursorFromText();
}

// Nueva función para restaurar el color de fondo original después de excepciones
void restore_normal_screen(void) {
    if (bg_color_saved) {
        // Restaurar el color de fondo original
        bg_color = saved_bg_color;
        bg_color_saved = 0;
        
        // Limpiar pantalla con el color original
        empty_screen(bg_color);
        
        // Re-renderizar el texto existente
        rewriteFromRawText();
    } else {
        // Si no hay color guardado, usar el color por defecto
        Color default_bg = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
        bg_color = default_bg;
        empty_screen(default_bg);
        rewriteFromRawText();
    }
}