#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#define IDT_DIM 256;

static void int_20();
static void int_21();

extern void (*irqHandlers[])(void);

void irqDispatcher(uint64_t irq);

#endif