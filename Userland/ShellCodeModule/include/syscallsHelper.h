#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} RegsSnapshot;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} time_struct;


// Syscalls básicas
int64_t sys_read(uint64_t fd, uint16_t * buffer, uint64_t amount);
int64_t sys_write(uint64_t fd, const char * buffer, uint64_t amount);

// Tiempo
int64_t sys_get_time(time_struct * time);
int64_t sys_nano_sleep(uint64_t nanos);  

// Registros
int64_t sys_get_registers(RegsSnapshot * regs);  

// Audio
int64_t sys_beep(uint64_t freq);  

// Video/pantalla
int64_t sys_set_font_size(uint64_t size);
int64_t sys_clear_screen(void);
int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color, uint64_t size);  
int64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color, uint64_t fill); 
int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size);  
int64_t sys_get_screen_info(void *info); 

// Teclado
unsigned char sys_get_key(void);

