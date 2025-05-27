#include "syscalls.h"

#define STDOUT 1
#define STDERR 2
#define STDOUT_FORMAT 0x0F
#define STDERR_FORMAT 0x0C

void sysCallDispatcher(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall_id) {
        case 4:
            sys_write(arg1, (char *)arg2, arg3);
            break;

        case 3:
            sys_read(arg1)
            break;
    
        default:
            break;
    }
}

void sys_write(uint64_t fd, char * buffer, uint64_t size){
    uint8_t format = (fd == STDOUT) ? STDOUT_FORMAT : 0x0F;
    for (int i = 0; i <= size; i++){
        ncPrintCharColor(buffer[i], format);
    }
}
