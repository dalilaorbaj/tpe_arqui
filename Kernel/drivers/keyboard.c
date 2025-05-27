#include <stdint.h>
#include <keyboard.h>

static uint16_t buffer[BUFFER_SIZE];
static uint64_t buffer_dim = 0;
static uint64_t buffer_current = 0;

int hasKey(){
    return in(0x64) & 1;
}

unsigned char getKey(){
    //espero a que haya algo
    while(!hasKey());
    
    return in(0x60);
} 

uint64_t bufferHasNext(){
    return ( buffer_dim > 0 ) && ( buffer_current < buffer_dim );
}

uint64_t getCurrent(){
    if(bufferHasNext()) {
        return buffer[buffer_current++];
    }
    return 0;
}