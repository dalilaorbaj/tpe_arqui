#include <stdint.h>

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rbp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15;
} Registers;

typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
} time_struct;

int64_t syscallDispatcher(Registers * regs);

int64_t sys_get_time(time_struct * time);

uint64_t sys_write(uint64_t fd, uint16_t * buf, uint64_t count);

uint64_t sys_read(uint64_t fd, uint16_t * buf, uint64_t count);