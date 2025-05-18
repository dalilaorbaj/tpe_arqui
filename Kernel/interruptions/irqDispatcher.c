#include <time.h>
#include <stdint.h>

/* Este código implementa un despachador de interrupciones para el kernel, específicamente para interrupciones IRQ.

La función irqDispatcher(uint64_t irq) recibe el número de interrupción (irq) y, usando un switch, decide qué función manejará esa interrupción.
Si el número de interrupción es 0, llama a la función int_20().
La función int_20() llama a timer_handler(), que normalmente maneja la interrupción del temporizador del sistema (timer tick). */

static void int_20();

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
	}
	return;
}

void int_20() {
	timer_handler();
}
