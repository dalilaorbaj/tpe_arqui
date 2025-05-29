#include <syscalls.h>
#include <video.h>
#include <keyboard.h>
#include <rtc.h>
#include <regs_snapshot.h>

#define STDOUT 1
#define STDERR 2
#define STDOUT_FORMAT 0x0F
#define STDERR_FORMAT 0x0C


// | Argumento número      | Registro usado |
// | --------------------- | -------------- |
// | 1 (primer argumento)  | `rdi`          |
// | 2 (segundo argumento) | `rsi`          |
// | 3 (tercer argumento)  | `rdx`          |
// | 4 (cuarto argumento)  | `rcx`          |
// | 5 (quinto argumento)  | `r8`           |
// | 6 (sexto argumento)   | `r9`           |


int64_t syscallDispatcher(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    switch (syscall_num) {
        case 0:
            return sys_get_time((time_struct *)arg1);
        case 1:
            return sys_set_font_size(arg1);
        case 2:
            return sys_nano_sleep(arg1);
        case 3:
            return sys_read(arg1, (char *)arg2, arg3);
        case 4:
            return sys_write(arg1, (char *)arg2, arg3);
        case 5:
            return sys_clear_screen();
        case 6:
            return sys_draw_pixel(arg1, arg2, arg3, arg4);
        case 7:
            return sys_draw_rectangle(arg1, arg2, arg3, arg4, arg5, arg6);
        case 8:
            return sys_draw_letter(arg1, arg2, arg3, arg4, arg5);
        case 9:
            return sys_get_screen_info((void *)arg1);
        case 10:
            return sys_get_registers((RegsSnapshot *)arg1);
        case 11:
            return sys_beep(arg1);
        case 20:
            return sys_get_key();
        default:
            return -1;
    }
}


//	unsigned int fd	char __user *buf	size_t count (de la syscall table de linux)
int64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count){
    if(fd != STDIN) return -1;
    int64_t i = 0;
    char c;
    while (i < count && (c = getchar()) != 0) {
        buf[i] = c;   
        i++;
    }
    return i;  
}


int64_t sys_write(uint64_t fd, const char * buf, uint64_t count){
    if (fd == 1) { // STDOUT
        return write(buf, count);
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

int64_t sys_clear_screen(void){
    Color bgColor = {0, 0, 0}; 
    empty_screen(bgColor);
    return 0;
}
