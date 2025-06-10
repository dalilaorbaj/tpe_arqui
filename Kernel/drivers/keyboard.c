#include <stdint.h>
#include <keyboard.h>

static uint64_t key_states[256] = {0}; 

static uint8_t buffer[BUFFER_SIZE];
static uint8_t map = LOWER;
static uint8_t buffer_dim = 0; 
static uint8_t buffer_last = 0; 
static uint8_t buffer_first = 0; 
static uint8_t registersFlag = 0; 
static uint8_t capsLock = 0;
static uint8_t cntrlPressed = 0;

extern int8_t keyMappingMatrix[KEY_MAPPING_MATRIX_ROWS][KEY_MAPPING_MATRIX_COLS];
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