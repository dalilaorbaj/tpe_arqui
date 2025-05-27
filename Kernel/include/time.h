#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timerHandler();
uint64_t ticksElapsed();
uint64_t secondsElapsed();
void sleepSecs(uint64_t secs);
void sleepTicks(uint64_t delayTicks);

#endif
