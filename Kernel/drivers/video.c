#include <video.h>
#include <font.h>

#define WIDTH VBE_info->width
#define HEIGHT VBE_info->height
#define CHAR_ROWS_FONT_MIN  48         
#define CHAR_COLS_FONT_MIN  128        
//#define Y_OFFSET (FONT_HEIGHT * font_size)
#define X_OFFSET (FONT_WIDTH * font_size)

static Point current_point = {0,0};
static Char buffer[CHAR_ROWS_FONT_MIN * CHAR_COLS_FONT_MIN];

static VBEInfoPtr VBE_info = (VBEInfoPtr) 0x0000000000005C00;
extern uint8_t font_bitmap[4096];
static uint64_t index = 0;
static uint8_t force_graphics = 0;  //para poder dibujar en modo texto

static Color bg_color = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
static Color font_color = {COLOR_ON, COLOR_ON, COLOR_ON};
static Color error_color = {COLOR_ON, COLOR_OFF, COLOR_OFF};
static uint64_t font_size = DEFAULT_FONT_SIZE;
static uint8_t driver_mode = TEXT_MODE;

static uint64_t CHAR_ROWS_FONT_MIN_WITH_ZOOM = CHAR_ROWS_FONT_MIN;
static uint64_t CHAR_COLS_FONT_MIN_WITH_ZOOM = CHAR_COLS_FONT_MIN;

static void empty_screen(Color new_bg_color);
static uint64_t inTextMode();
static uint64_t inVideoMode();
static void tab();
static void printLetter(Char letter);
static void nl();
static void bs();
static void printAgain();
static void print();



int64_t set_font_size(uint64_t new_font_size) {
    if( !inTextMode() || size < 0 || WIDTH < size * FONT_WIDTH || HEIGHT < size * FONT_HEIGHT)
        return ERROR;

    if (size == new_font_size)
        return EARLY_EXIT;

    new_font_size = size;
    CHAR_ROWS_FONT_MIN_WITH_ZOOM = (HEIGHT / (new_font_size * FONT_HEIGHT));
    CHAR_COLS_FONT_MIN_WITH_ZOOM = (WIDTH / (new_font_size * FONT_WIDTH));
    emptyScreen(bg_color);
    return OK;
}

static void empty_screen(Color new_bg_color){
    if(driver_mode == TEXT_MODE){
        bg_color = new_bg_color;
        current_point.x = current_point.y = 0;
        index = 0;
    }
    force_graphics = 1;
    draw_rectangle(0, 0, WIDTH, HEIGHT, new_bg_color);
    force_graphics = 0;
}

int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, Color color){
    if(x+width > WIDTH || y+height > HEIGHT || !inVideoMode()) return ERROR;
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

void print_in_screen(char * message, char fmt){
    char * v = (char *) 0xB8000;
    for(int i=0 ; s[i] != '\0' ; i++){
        *v = s[i];
        *(v + 1) = fmt;
        v += 2;
    }
}

int64_t set_driver_mode(uint64_t mode, Color color){
    if(mode != TEXT_MODE && mode != VIDEO_MODE) return ERROR;
    bg_color = color;   
    if(mode == driver_mode ) return EARLY_EXIT; //cambio el color pero el modo queda como estaba
    driver_mode = mode;

    if(mode==TEXT_MODE) print();
    else empty_screen(color);

    return OK;
}

int64_t write(uint64_t fd, const char * buffer, int64_t size){
    if(!inTextMode() || (fd!=STDOUT && fd!=STDERR)) return EARLY_EXIT;

    uint64_t i = 0;
    while (buffer[i] != 0 && i<size) {
        switch (buffer[i]) {
            case '\n':
                nl();
                break;
            case '\b':
                bs();
                break;
            case '\t':
                tab();
                break;
            default:
                if(buffer[i] >= FIRST_ASCII && buffer[i] <=LAST_ASCII){
                    Char c = {buffer[i], fd};
                    append(buffer[i], fd);
                    printLetter(c);
                }
                break;
        }
        i++;
    }
    return i;
}

static void nl(){
    for(int i=0 ; i<CHAR_COLS_FONT_MIN_WITH_ZOOM - (index % CHAR_COLS_FONT_MIN_WITH_ZOOM); i++){
        append(' ', STDOUT);
    }
    current_point.x = 0;
    current_point.y = current_point.y + FONT_HEIGHT * font_size;
    if(current_point.y + FONT_HEIGHT * font_size - FONT_HEIGHT >= HEIGHT){
        printAgain();
    }
}

static void bs(){
    if(current_point.x == 0 && current_point.y == 0){
        return;
    }
    if (current_point.y < font_size * FONT_HEIGHT) {
        current_point.y = 0;
    }
    if(current_point.x == 0){
        current_point.y -= font_size*FONT_HEIGHT;  
        current_point.x = WIDTH - font_size*FONT_WIDTH;
    }
    current_point.x -= font_size*FONT_WIDTH;
    
    force_graphics=1;
    draw_rectangle(current_point.x , current_point.y, X_OFFSET, FONT_HEIGHT*font_size, bg_color);
    force_graphics=0;    
        if(index != 0){
        buffer[index].c = 0;
        buffer[--index].fd = STDOUT;
    }
}

static void tab(){
    Char c = {' ', STDOUT};
    for(int i=0; i<3 ; i++){
        append(' ', STDOUT);
        printLetter(c);
    }
}

int64_t draw_font(uint64_t x, uint64_t y, uint8_t letter, Color color,uint64_t fontSize){
    if(letter < FIRST_ASCII || letter > LAST_ASCII){
        return ERROR;
    }
    if(!inVideoMode()) return EARLY_EXIT;
    int letter = (letter-' ')*16;
    for (uint64_t i = 0; i < 16; i++) {
        for (uint64_t j = 0; j < 8; j++) {
            if((font_bitmap[letter+i] >> (7-j)) & 0x1){
                draw_rectangle((x +j*fontSize), (y +i*fontSize),fontSize,fontSize, color);
            }
        }
    }
    return OK;
}

int64_t draw_pixel(uint64_t x, uint64_t y, Color color){
    if(x >= WIDTH || y>= HEIGHT || !inVideoMode()) return ERROR;

    uint8_t * vram = (uint8_t *) VBE_info->vram;
    uint64_t index = (x * ((VBE_info->bpp)/8)) + (y * VBE_info->pitch);
    vram[index] = color.b;
    vram[index+1] = color.g;
    vram[index+2] = color.r;
    return OK;
}

static uint64_t inTextMode(){
    return ((driver_mode == TEXT_MODE ) || force_graphics);
}
static uint64_t inVideoMode(){
    return ((driver_mode==VIDEO_MODE) || force_graphics);
}

static void printLetter(Char letter){
    if (current_point.x+FONT_WIDTH*font_size - FONT_WIDTH >= WIDTH) {
        current_point.y += FONT_HEIGHT*font_size;
        current_point.x = 0;
    }
    if(current_point.y + FONT_HEIGHT*font_size - FONT_HEIGHT >= HEIGHT){
        printAgain();
    }
    force_graphics=1;
    Color color;
    if(letter.fd == STDOUT){
        color = font_color;
    }else{
        color = error_color;
    }
    draw_font(current_point.x, current_point.y, letter.c, color, font_size);
    force_graphics=0;
    current_point.x+=FONT_WIDTH*font_size;
}

static void printAgain(){
    uint64_t aux = index;
    emptyScreen();
    uint64_t j = 0;
    for(uint64_t i=CHAR_COLS_FONT_MIN_WITH_ZOOM * ((CHAR_ROWS_FONT_MIN_WITH_ZOOM/2)+1) ; i<aux;i++, j++){
        char_buffer[j] = char_buffer[i];
        printLetter(char_buffer[j]);
    }
    index = j;
}

static void print(){
    uint64_t aux = index;
    emptyScreen();
    for(int i=0 ; i<aux ; i++){
        write(char_buffer[i].fd, &char_buffer[i].c, 1);
    }
}

void set_font_color(Color color){
    font_color=color;
}