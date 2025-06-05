#include <pongisGolf.h>

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

void startExceptionScreen(){
    if(get_screen_info(&screen) <0 ){
        puts("Failed to get screen info.");
        return;
    }

    baseY = screen.height / 3;
    verticalSpacing = FONT_HEIGHT * 3;
    currentY = baseY;
    screenWidth = screen.width;
    screenHeight = screen.height;

    draw_rectangle(0, 0, screen.width, screen.height, 0x8B1C2C);

    printString("Choose how many players: ", screen.width/2, currentY, 3, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing);

    //print la info de la syscall, snapshot, etc
    //moveCurrentY(verticalSpacing - 10);

    printString("Press 'e' or 'E' to return to shell", screen.width/2, currentY, 2, screen.width, screen.height, ALIGN_CENTER);
    moveCurrentY(verticalSpacing - 10);

    char c;
    do {
        c = getChar();
    
        if (c == 'e' || c == 'E') {
            setExit();
            return;
        } else {
            printString("Invalid key.", screen.width / 2, screen.height - 30, 2, screen.width, screen.height, ALIGN_CENTER);
        }
    } while (1);
    return;
}


static void setExit(){
    clearScreen();
    printString("Exiting Exception Screen...", screenWidth/2, screenHeight/2, 2, screenWidth, screenHeight, ALIGN_CENTER);
    sys_nano_sleep(40); // 1 segundo
    clearScreen();
    flushKeyboardBuffer();
}