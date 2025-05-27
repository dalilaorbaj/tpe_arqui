#include <time.h>
#include <stdint.h>

/* Este código implementa un despachador de interrupciones para el kernel, específicamente para interrupciones IRQ.

La función irqDispatcher(uint64_t irq) recibe el número de interrupción (irq) y, usando un switch, decide qué función manejará esa interrupción.
Si el número de interrupción es 0, llama a la función int_20().
La función int_20() llama a timer_handler(), que normalmente maneja la interrupción del temporizador del sistema (timer tick). */


static void int_20();

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

// cambiar el switch por un arreglo de punteros a funcion
void irqDispatcher(uint64_t irq) {
	irqHandlers[irq]();
}

void int_20() {
	timer_handler();
}

// agregar el handler en interrupts.asm
// agregarlo mediante el idt loader
