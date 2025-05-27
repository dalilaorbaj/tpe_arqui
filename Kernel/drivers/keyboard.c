#include <stdint.h>
#include <Keyboard.h>

unsigned char in(unsigned short port); //devuelve el valor leido del puerto 64
void out(unsigned short port, unsigned char value);


int hasKey(){
    return inb(0x64) & 1;
}

unsigned char getKey(){
    //espero a que haya algo
    while(!hasKey());
    
    return in(0x60);
}