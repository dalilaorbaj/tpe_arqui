#include <syscalls.h>
#include <naiveConsole.h>
#include <video.h>
#include <keyboard.h>
#include <rtc.h>

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

int64_t syscallDispatcher(Registers * registers) {
    switch (registers->rax) {   //en rax esta la syscall id
        case 4:
            sys_write(registers->rdi, (char *)registers->rsi, registers->rdx);
            break;

        case 3:
            sys_read(registers->rdi, (char *)registers->rsi, registers->rdx);
            break;
    
        //aca vamos a tener que poner todo el resto de syscalls

        default:
            return ERROR;
            break;
    }
}

//	unsigned int fd	char __user *buf	size_t count (de la syscall table de linux)
int64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count){
    if (buf == 0 || count == 0) {
        return ERROR;
    }
    uint64_t i = 0;
    while (i < count && bufHasNext()) {
        buf[i++] = getBufCurrent();
    }
    return i;  
}


//text_mode
int64_t sys_write(uint64_t fd, uint16_t * buf, uint64_t count){
    return write(buf, count);
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