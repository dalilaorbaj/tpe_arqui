#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H


static void int_20();
static void int_21()


// Array of function pointers for IRQ handlers
static void (*irqHandlers[])(void) = {
	int_20,    // IRQ 0 - Timer
	NULL,      // IRQ 1
	NULL,      // IRQ 2
	NULL,      // IRQ 3
	NULL,      // IRQ 4
	NULL,      // IRQ 5
	NULL,      // IRQ 6
	NULL       // IRQ 7
	// Add more NULL entries if you need to handle more IRQs
};

void irqDispatcher(uint64_t irq);

#define IDT_DIM = 256;

#endif