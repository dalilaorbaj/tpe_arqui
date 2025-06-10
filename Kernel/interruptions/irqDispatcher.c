#include <time.h>
#include <stdint.h>
#include <irqDispatcher.h>
#include <stddef.h>
#include <keyboard.h>

static void int_20();
static void int_21();
void _exception_invalidOpcodeHandler(void);

static void (*irqHandlers[])(void) = {
	int_20,    // IRQ 0 - Timer
	int_21,      // IRQ 1 - Keyboard
	//int_06,      // IRQ 3
	NULL, 
	NULL,
	NULL,      // IRQ 4
	NULL,      // IRQ 5
	NULL,      // IRQ 6
	NULL       // IRQ 7
};


void irqDispatcher(uint64_t irq) {
	if (irq < IDT_DIM && irqHandlers[irq] != NULL) {
		irqHandlers[irq]();
	}
}

static void int_20() {
	timerHandler();
}

static void int_21(){
	keyboardHandler();
}

// //(?) che que onda esto?
// static void int_06(){
// 	//invalidOpcodeHandler();
// }