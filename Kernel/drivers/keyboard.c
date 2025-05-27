#include <stdint.h>
#include <keyboard.h>


int hasKey(){
    return inb(0x64) & 1;
}

unsigned char getKey(){
    //espero a que haya algo
    while(!hasKey());
    
    return in(0x60);
} 