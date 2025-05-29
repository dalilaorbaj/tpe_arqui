#include <syscalls.h>
#include <naiveConsole.h>
#include <video.h>
#include <keyboard.h>
#include <rtc.h>
#include <regs_snapshot.h>

#define STDOUT 1
#define STDERR 2
#define STDOUT_FORMAT 0x0F
#define STDERR_FORMAT 0x0C

extern unsigned char sys_get_key(void);
int64_t sys_set_font_size(uint64_t size);
int64_t sys_nano_sleep(uint64_t nanos);
int64_t sys_clear_screen(void);
int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color, uint64_t size);
int64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color, uint64_t fill);
int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size);
int64_t sys_get_screen_info(void *info);
int64_t sys_get_registers(RegsSnapshot *regs);
int64_t sys_beep(uint64_t freq);


// | Argumento número      | Registro usado |
// | --------------------- | -------------- |
// | 1 (primer argumento)  | `rdi`          |
// | 2 (segundo argumento) | `rsi`          |
// | 3 (tercer argumento)  | `rdx`          |
// | 4 (cuarto argumento)  | `rcx`          |
// | 5 (quinto argumento)  | `r8`           |
// | 6 (sexto argumento)   | `r9`           |

int64_t syscallDispatcher(Registers * registers) {
    switch (registers->rax) {
        case 0:
            registers->rax = sys_get_time((time_struct *)registers->rdi);
            break;
        case 1:
            registers->rax = sys_set_font_size(registers->rdi);
            break;
        case 2:
            registers->rax = sys_nano_sleep(registers->rdi);
            break;
        case 3:
            registers->rax = sys_read(registers->rdi, (void *)registers->rsi, registers->rdx);
            break;
        case 4:
            registers->rax = sys_write(registers->rdi, (void *)registers->rsi, registers->rdx);
            break;
        case 5:
            registers->rax = sys_clear_screen();
            break;
        case 6:
            registers->rax = sys_draw_pixel(registers->rdi, registers->rsi, registers->rdx, registers->rcx);
            break;
        case 7:
            registers->rax = sys_draw_rectangle(registers->rdi, registers->rsi, registers->rdx, registers->rcx, registers->r8, registers->r9);
            break;
        case 8:
            registers->rax = sys_draw_letter(registers->rdi, registers->rsi, registers->rdx, registers->rcx, registers->r8);
            break;
        case 9:
            registers->rax = sys_get_screen_info((void *)registers->rdi);
            break;
        case 10:
            registers->rax = sys_get_registers((RegsSnapshot *)registers->rdi);
            break;
        case 11:
            registers->rax = sys_beep(registers->rdi);
            break;
        case 20:
            registers->rax = sys_get_key();
            break;
        default:
            return ERROR;
    }
    return 0;
}


//	unsigned int fd	char __user *buf	size_t count (de la syscall table de linux)
int64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count){
    if (buf == 0 || count == 0) {
        return ERROR;
    }
    uint64_t i = 0;
    while (i < count && bufferHasNext()) {
        buf[i++] = getCurrent();
    }
    return i;  
}


int64_t sys_write(uint64_t fd, const char * buf, uint64_t count){
    if (fd == 1) { // STDOUT
        return write((const char *)buf, count);
    }
    return -1;
}


int64_t sys_get_time(time_struct * time){
    time->seconds = getRTCSeconds();
    time->minutes =  getRTCMinutes();
    time->hour =  getRTCHours();
    time->day = getRTCDayOfMonth();
    time->month = getRTCMonth();
    time->year = getRTCYear();
    return 0;
}

unsigned char sys_get_key(void) {
    return getKey();
}