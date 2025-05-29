// #include <stdint.h>
// #include <keyboard1.h>
// #include <keys.h>

/* El tratamiento del buffer de teclado es circular */
/* static uint16_t buffer[BUFFER_SIZE];
static uint64_t buffer_dim = 0; //cantidad de elementos en el buffer
static uint64_t buffer_last = 0; //ultimo elemento agregado al buffer
static uint64_t buffer_first = 0; //primer elemento del buffer
static int shift_pressed = 0;

extern uint8_t sys_getKey();

#define PRESSED_KEY_SHIFT_MAP_SIZE 128

static uint8_t handlekey(uint8_t key){
    if (key >= PRESSED_KEY_SHIFT_MAP_SIZE) {
        return 0; // Invalid key
    }
    
    // Check if the key is a special key
    if (key < FIRST_SPECIAL_KEY || key > LAST_SPECIAL_KEY) {
        return pressedKeyShiftMap[key][shift_pressed];
    }
    
    // Return the special key value
    return pressedKeyShiftMap[key - FIRST_SPECIAL_KEY][shift_pressed];
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




void keyboardHandler() {
    uint8_t key = handlekey(sys_getKey());
    
    if (key == 0) {
        return; // No key pressed
    }

    //Agregamos la tecla al buffer
    buffer[bufferLast++] = key;
    bufferLast = bufferLast % BUFFER_SIZE;
    bufferElementCount++;
    return;
} */