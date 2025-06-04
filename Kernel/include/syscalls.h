#include <stdint.h>
#include "video.h"
#include "regs_snapshot.h"

typedef struct time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} time_struct;

int64_t syscallDispatcher(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);
int64_t sys_get_time(time_struct * time);
int64_t sys_write(uint64_t fd, const char * buf, uint64_t count);
int64_t sys_write_color(uint64_t fd, const char * buf, uint64_t count, uint64_t color);
int64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count);
unsigned char sys_get_key(void);
int64_t sys_set_font_size(uint64_t size);
int64_t sys_nano_sleep(uint64_t nanos);
int64_t sys_clear_screen(void);
int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color);
int64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);
int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size);
int64_t sys_get_screen_info(Screen * screen);
int64_t sys_get_registers(RegsSnapshot *regs);
int64_t sys_beep(uint64_t freq, uint64_t time);
int64_t sys_is_key_pressed(uint8_t scancode);