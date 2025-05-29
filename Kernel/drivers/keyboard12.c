/* #include <stdint.h>
#include <keyboard1.h>
#include <keys.h>

static uint16_t buffer[BUFFER_SIZE];
static uint64_t buffer_dim = 0;
static uint64_t buffer_current = 0;

extern uint8_t sys_getKey();

#define PRESSED_KEY_SHIFT_MAP_SIZE 128

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

uint16_t getCurrent(){
    if(bufferHasNext()) {
        return buffer[buffer_current++];
    }
    return 0;
}

// void keyboardHandler() {
//     if (hasKey()) {
//         unsigned char scancode = in(0x60);
        
//         // Solo agregar al buffer si no está lleno
//         if (buffer_dim < BUFFER_SIZE) {
//             buffer[buffer_dim++] = scancode;
//         }
//     }
// }

static int shift_pressed = 0;

void keyboardHandler() {
    if (hasKey()) {
        unsigned char scancode = in(0x60);

        // Manejo de shift presionado/liberado
        if (scancode == 0x2A || scancode == 0x36) { // Shift presionado
            shift_pressed = 1;
            return;
        }
        if (scancode == 0xAA || scancode == 0xB6) { // Shift liberado
            shift_pressed = 0;
            return;
        }

        if (scancode & 0x80)
            return;

        // Solo agregar al buffer si no está lleno y el scancode es válido
        if (buffer_dim < BUFFER_SIZE && scancode > 0 && scancode < PRESSED_KEY_SHIFT_MAP_SIZE) {
            char ascii = pressedKeyShiftMap[scancode][shift_pressed];
            if (ascii) { // Solo guardamos caracteres imprimibles
                buffer[buffer_dim++] = ascii;
            }
        }
    }
} */