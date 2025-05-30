#include <syscalls.h>
#include <video.h>
#include <keyboard.h>
#include <rtc.h>
#include <regs_snapshot.h>
#include <sound.h>
#include <stddef.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STDOUT_FORMAT ((Color){255, 255, 255}) // White color for standard output

extern void get_snapshot();
extern uint64_t registersArray[18];


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
    Color bgColor = {0, 0, 0}; 
    empty_screen(bgColor);
    return 0;
}

// int64_t sys_beep(uint32_t freq, int time) {
//     if (time == 0 || freq < 20 || freq > 20000) {
//         return -1;
//     }
//     beep(freq, time);
//     return 0;
// }

int64_t sys_beep(uint64_t freq, uint64_t time) {
    sys_write(STDOUT, "[sys_beep] called\n", 17);
    if (time == 0 || freq < 20 || freq > 20000) {
        sys_write(STDOUT, "[sys_beep] invalid args\n", 24);
        return -1;
    }
    beep(freq, time);
    sys_write(STDOUT, "[sys_beep] finished beep\n", 24);
    return 0;
}


int64_t sys_get_registers(RegsSnapshot *regs) {
    if (regs == NULL) {
        return -1; 
    }

    
    get_snapshot();
    
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
    return 0;
}

// PENDIENTE
int64_t sys_set_font_size(uint64_t size) { return 0; }


int64_t sys_draw_pixel(uint64_t x, uint64_t y, uint64_t color) { 
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    return draw_pixel(x, y, c);
}
/*
void scale_screen(uint64_t factor) {
    // Crear buffer temporal
    uint8_t* temp_buffer = allocate_screen_buffer();
    
    // Copiar pantalla actual
    copy_framebuffer_to_buffer(temp_buffer);
    
    // Limpiar pantalla
    clear_screen();
    
    // Re-dibujar escalado
    for (uint32_t y = 0; y < screen_height; y += factor) {
        for (uint32_t x = 0; x < screen_width; x += factor) {
            Color pixel = get_pixel_from_buffer(temp_buffer, x/factor, y/factor);
            draw_scaled_pixel(x, y, pixel, factor);
        }
    }
    
    free_buffer(temp_buffer);
}
    */
int64_t sys_draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size) { 
    Color c = { (uint8_t)(color >> 16), (uint8_t)(color >> 8), (uint8_t)color };
    return draw_font(x, y, (char)letter, c, size);
}

int64_t sys_get_screen_info(Screen *info) {
    //esto no se si va 
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