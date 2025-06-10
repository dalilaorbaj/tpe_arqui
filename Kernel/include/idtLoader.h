#ifndef _IDTLOADER_H_
#define _IDTLOADER_H_

#include <defs.h>
#include <interrupts.h>

void load_idt();

#define ACS_INT       0x8E    // P=1, DPL=0, type=14 (interrupt gate)
#define ACS_SYSCALL   0xEE    // P=1, DPL=3, type=14 (interrupt gate)

#endif