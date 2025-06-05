#include <stdint.h>
#include <video.h>
#include <idtLoader.h>
#include <syscalls.h>

#define STDERR 1

#define CANT_REGS 18
uint64_t exception_regs[CANT_REGS];

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6


static void zero_division();

static void zero_division() {
	// Handler para manejar excepcíon
}

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
}


