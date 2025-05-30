#include <pongisGolf.h>
#include <library.h>

static void singlePLayer();
static void multiPlayer();




void startPongisGolf(){
    puts("Starting Pongis Golf...");
    clearScreen();
    Screen screen;
    if(sys_get_screen_info(&screen) != 0){
        puts("Failed to get screen info.");
        return;
    }
    draw_rectangle(0, 0, screen.width, screen.height, 0x00FF00); // Fill the screen with green
    //printText();
    char map[screen.height][screen.width];


}


static void printString(const char *str, uint64_t x, uint64_t y, uint64_t size) {
    if (str == NULL || x >= WIDTH || y >= HEIGHT) {
        return; // Invalid input
    }
    uint64_t i = 0;
    while (str[i] != '\0' && x + i * size < WIDTH) {
        draw_font(x + i * CHARACTER_WIDTH * size, y, str[i], (Color){0, 255, 0}, size); // Draw each character in green
        i++;
    }
}