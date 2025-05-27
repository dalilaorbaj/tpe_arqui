#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#define IDT_DIM 256


//void (*irqHandlers[])(void);

void irqDispatcher(uint64_t irq);

#endif