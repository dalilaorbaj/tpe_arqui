#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

#define DEFAULT_FONT_SIZE 1
#define COLOR_OFF 0
#define COLOR_ON 255

#define ERROR -1
#define EARLY_EXIT 0
#define OK 1

#define STDIN   0
#define STDOUT  1
#define STDERR  2

typedef struct{
    uint64_t x;
    uint64_t y;
}Point;

typedef struct{
    uint8_t c;
    uint8_t fd;
}Character;

typedef struct{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
} Screen;

typedef struct vbe_mode_info_structure * VBEInfoPtr;

int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, Color color);
int64_t get_screen_info(Screen * screen); 
//uint64_t write(const char * buf, int64_t size, Color color);
int64_t draw_font(uint64_t x, uint64_t y, uint8_t ch, Color color, uint64_t size); 
int64_t draw_pixel(uint64_t x, uint64_t y, Color color);
void set_font_color(Color color);
void empty_screen(Color newBgColor);


struct vbe_mode_info_structure {
    uint16_t attributes;	// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;		// deprecated
    uint8_t window_b;		// deprecated
    uint16_t granularity;	// deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;	// deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;			// number of bytes per horizontal line
    uint16_t width;			// width in pixels
    uint16_t height;		// height in pixels
    uint8_t w_char;			// unused...
    uint8_t y_char;			// ...
    uint8_t planes;
    uint8_t bpp;			// bits per pixel in this mode
    uint8_t banks;			// deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed));

#endif