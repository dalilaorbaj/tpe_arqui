#include <stdint.h>

typedef struct registers {
    uint64_t rflags, rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rip;
} RegsSnapshot;

typedef struct{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
}time_struct;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
} Screen;

// Syscalls básicas
int64_t sys_read(uint64_t fd, uint16_t * buffer, uint64_t amount);
int64_t sys_write(uint64_t fd, const char * buffer, uint64_t amount);
int64_t sys_write_color(uint64_t fd, const char * buf, uint64_t count, Color color);

// Tiempo
int64_t sys_get_time(time_struct * time);
int64_t sys_nano_sleep(uint64_t nanos);  

// Registros
int64_t sys_get_registers(RegsSnapshot * regs); 

// Sound
int64_t sys_beep(uint64_t freq, uint64_t time);  

// Video/pantalla
int64_t sys_set_font_size(uint64_t size);
int64_t sys_clear_screen(void);
int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color, uint64_t size);  
int64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color); 
int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size);  
int64_t sys_get_screen_info(Screen *info); 

// Teclado
unsigned char sys_get_key(void);

