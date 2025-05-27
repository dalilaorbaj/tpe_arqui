#include <time.h>
#include <stdint.h>
#include <irqDispatcher.h>

/* Este código implementa un despachador de interrupciones para el kernel, específicamente para interrupciones IRQ.

La función irqDispatcher(uint64_t irq) recibe el número de interrupción (irq) y, usando un switch, decide qué función manejará esa interrupción.
Si el número de interrupción es 0, llama a la función int_20().
La función int_20() llama a timerHandler(), que normalmente maneja la interrupción del temporizador del sistema (timer tick). */


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

static void int_06(){
	invalidOpcodeHandler();
}

//creo que el 22 no se usa por er la linea en cascada del PIC master/slave
// static void int_22(){

// }

//NO SE SI HACE FALTA ESTO
// static void int_23(){
// 	serialPort2Handler();
// }

// static void int_24() {
//     serialPort1Handler();
// }

//esto creo que si lo necesitamos por el tema del pendrive
static void int_25() {
    usbHandler();
}

// agregar el handler en interrupts.asm
// agregarlo mediante el idt loader
