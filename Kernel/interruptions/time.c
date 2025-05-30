#include <time.h>
#include <lib.h>

#define TICKS_PER_SEC 18
static uint64_t ticksCount = 0;

void timerHandler() {
	ticksCount++;
}

uint64_t ticksElapsed() {
	return ticksCount;
}

uint64_t secondsElapsed() {
	return ticksCount / TICKS_PER_SEC;
}

void sleepTicks(uint64_t delayTicks) {
    uint64_t start = ticksCount;
    while ((ticksCount - start) < delayTicks) {
        _hlt();
    }
}

void sleepSecs(uint64_t secs) {
    sleepTicks(secs * TICKS_PER_SEC);
}
