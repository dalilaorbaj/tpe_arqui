#include <stdint.h>
#include <keyboard.h>

#define BUFFER_SIZE 1024
#define UNPRESSED_BIT   0x80 //indica si la tecla esta presionada o no
#define UPPER 1
#define LOWER 0
#define KEY_BITS_MASK   0x7F // mascara para obtener el valor de la tecla sin el bit de presionada
#define LSHIFT          0x2A
#define RSHIFT          0x36
#define CPS_LOCK        0x3A
#define SNAPSHOT_KEY    0x43 // tecla para tomar un snapshot de los registros
#define RIGHT_CONTROL   0xE01D
#define LEFT_CONTROL    0x1D
#define DELTA ('a' - 'A')

static uint64_t key_states[256] = {0}; // Array para rastrear estado de teclas

/* El tratamiento del buffer de teclado es circular */
static uint8_t buffer[BUFFER_SIZE];
static uint8_t map = LOWER; //mapa de teclas actual
static uint8_t buffer_dim = 0; //cantidad de elementos en el buffer
static uint8_t buffer_last = 0; //ultimo elemento agregado al buffer
static uint8_t buffer_first = 0; //primer elemento del buffer
static uint8_t registersFlag = 0; //bandera para indicar si se debe guardar los registros
static uint8_t capsLock = 0;
static uint8_t cntrlPressed = 0;

extern int8_t keyMappingMatrix[2][128];

extern uint8_t sys_getKey();

static uint8_t isReleased(uint8_t key){
    return (key & UNPRESSED_BIT);
}

static uint8_t isPressed(uint8_t key){
    return !isReleased(key);
}

static uint8_t keyValue(uint8_t key) {
    return (key & KEY_BITS_MASK);
}

static int isControlKey(uint8_t key){
    return (keyValue(key) == LEFT_CONTROL || keyValue(key) == RIGHT_CONTROL);
}

static int isShiftKey(uint8_t key){
    return (keyValue(key) == LSHIFT || keyValue(key) == RSHIFT);
}

static int isCapsLock(uint8_t key) {
    return (keyValue(key) == CPS_LOCK);
}

static uint8_t toUpper(uint8_t key) {
    return 'a' <= key && key <= 'z' ? key - DELTA : key;
}

static uint8_t toLower(uint8_t key) {
    return 'A' <= key && key <= 'Z' ? key + DELTA : key;
}

static int isAlphaKey(uint8_t key){
    return 'A' <= toUpper(key) && toUpper(key) <= 'Z';
}

static uint8_t handlekey(uint8_t key){
    uint8_t scancode = keyValue(key);
    // Actualizar estado de la tecla
    key_states[scancode] = isPressed(key);

    registersFlag = 0;
    if(isControlKey(key)) {
        cntrlPressed = (isPressed(key));
        return 0;
    }
    if(isShiftKey(key)) {
        map = (isPressed(key));
        return 0;
    }
    if(isCapsLock(key) && isPressed(key)) {
        capsLock = !capsLock;
        return 0;
    }

    if (key == SNAPSHOT_KEY && isPressed(key)) {
        getSnapshot();
    } 

    if(!isPressed(key) || keyMappingMatrix[map][key] == 0) return 0;    

    key = keyMappingMatrix[map][key];
    if(isAlphaKey(key) && capsLock) {
        key = (map == LOWER)? toUpper(key):toLower(key);
    }
    return key;
}


uint8_t getKey() {
    uint8_t toReturn;
    do {
        toReturn = handlekey(sys_getKey());
    } while (toReturn == 0);    
    return toReturn;
}

int emptyBuffer(){
    return buffer_dim == 0;
}


void keyboardHandler() {
    uint8_t key = handlekey(sys_getKey());
    
    if (key == 0) {
        return; 
    }
    //Agregamos la tecla al buffer
    buffer[buffer_last++] = key;
    buffer_last = buffer_last % BUFFER_SIZE;
    buffer_dim++;
    return;
}

uint64_t is_key_currently_pressed(uint8_t scancode) {
    return key_states[scancode];
}
static uint8_t next(){
    if (emptyBuffer()){
        return 0;
    }
    buffer_dim--;
    uint8_t toReturn = buffer[buffer_first++];
    buffer_first = buffer_first % BUFFER_SIZE;
    return toReturn;
}

uint8_t getChar(){
    return next();
}

