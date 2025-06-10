#include <syscalls.h>
#include <video.h>
#include <keyboard.h>
#include <rtc.h>
#include <regs_snapshot.h>
#include <sound.h>
#include <stddef.h>

#define STDOUT_FORMAT ((Color){255, 255, 255}) // White color for standard output

int64_t registersArray[CANT_REGS]; 
int64_t registersArrayAux[CANT_REGS];
int64_t registersArrayException[CANT_REGS];

int64_t syscallDispatcher(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    switch (syscall_num) {
        case 0:
            return sys_get_time((time_struct *)arg1);
        case 1:
            return sys_is_key_pressed(arg1);
        case 2:
            return sys_nano_sleep(arg1);
        case 3:
            return sys_read(arg1, (uint16_t *)arg2, arg3);
        case 4:
            return sys_write(arg1, (char *)arg2, arg3);
        case 5:
            return sys_clear_screen();
        case 6:
            return sys_draw_pixel(arg1, arg2, arg3);
        case 7:
            return sys_draw_rectangle(arg1, arg2, arg3, arg4, arg5);           
        case 8:
            return sys_draw_letter(arg1, arg2, arg3, arg4, arg5);
        case 9:
            return sys_get_screen_info((Screen *)arg1);
        case 10:
            return sys_get_registers((RegsSnapshot *)arg1);
        case 11:
            return sys_beep(arg1, arg2);
        case 12:
            return sys_write_color(arg1, (char *)arg2, arg3, arg4); 
        case 13:
            return sys_zoom_in();
        case 14:
            return sys_zoom_out();
        case 20:
            return sys_get_key();
        case 21:
            return sys_switch_text_mode(arg1);
        default:
            return -1;
    }
}

int64_t sys_is_key_pressed(uint8_t scancode) {
    return is_key_currently_pressed(scancode);
}

int64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count){
    if(fd != STDIN) return -1;
    int64_t i = 0;
    uint8_t c;
    while (i < count && (c = getChar()) != 0) {
        buf[i] = c;   
        i++;
    }
    return i;  
}


int64_t sys_write(uint64_t fd, const char * buf, uint64_t count){
    if (fd == STDOUT || fd == STDERR) {
        return write(buf, count, STDOUT_FORMAT);
    }
    return -1;
}

int64_t sys_write_color(uint64_t fd, const char * buf, uint64_t count, uint64_t color) {
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    if (fd == STDOUT || fd == STDERR) {
        return write(buf, count, c);
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
    clear_video_buffers();
    return 0;
}

int64_t sys_beep(uint64_t freq, uint64_t time) {
    if (time == 0 || freq < 20 || freq > 20000) {
        sys_write(STDOUT, "[sys_beep] invalid args\n", 24);
        return -1;
    }
    beep(freq, time);
    return 0;
}


int64_t sys_get_registers(RegsSnapshot *regs) {
    if (regs == NULL) {
        return -1; 
    }

    regs->rflags = registersArray[0];
    regs->rax    = registersArray[1];
    regs->rbx    = registersArray[2];
    regs->rcx    = registersArray[3];
    regs->rdx    = registersArray[4];
    regs->rsi    = registersArray[5];
    regs->rdi    = registersArray[6];
    regs->rbp    = registersArray[7];
    regs->rsp    = registersArray[8];
    regs->r8     = registersArray[9];
    regs->r9     = registersArray[10];
    regs->r10    = registersArray[11];
    regs->r11    = registersArray[12];
    regs->r12    = registersArray[13];
    regs->r13    = registersArray[14];
    regs->r14    = registersArray[15];
    regs->r15    = registersArray[16];
    regs->rip    = registersArray[17];
    regs->cs     = registersArray[18];
    regs->ss     = registersArray[19];
    return 0;
}




int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color) { 
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    return draw_pixel(x, y, c);
}

int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size) { 
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    return draw_font(x, y, (char)letter, c, size);
}

int64_t sys_get_screen_info(Screen *info) {
    if (info == NULL) {
        return -1;
    }
    
    return get_screen_info(info);
}

int64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color) { 
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    return draw_rectangle(x, y, width, height, c);
}

int64_t sys_nano_sleep(uint64_t nanos) { 
    sleepTicks(nanos);
    return 0;
}

int64_t sys_zoom_in(void) {
    increaseFontSize();
    return 0;
}

int64_t sys_zoom_out(void) {
    decreaseFontSize();
    return 0;
}

int64_t sys_switch_text_mode(uint64_t mode) {
    switch (mode){
        case 0: switch_to_default_font(); break;
        case 1: switch_to_dyslexic_font(); break;
        default: return -1; // Invalid mode
    }
    return 0;
}


void getSnapshot(){
    for(int i=0 ; i < CANT_REGS ; i++){
        registersArray[i] = registersArrayAux[i];
    }
}