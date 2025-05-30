// #include <stdint.h>
// #include <keyboard1.h>
// #include <keys.h>

// #define UNPRESSED_BIT   0x80 //indica si la tecla esta presionada o no
// #define SAVE_REGS_KEY F7   //tecla para guardar los registros
// #define UPPER 1
// #define LOWER 0
// #define KEY_BITS_MASK   0x7F // mascara para obtener el valor de la tecla sin el bit de presionada


/* El tratamiento del buffer de teclado es circular */
/* static uint16_t buffer[BUFFER_SIZE];
static uint64_t buffer_dim = 0; //cantidad de elementos en el buffer
static uint64_t buffer_last = 0; //ultimo elemento agregado al buffer
static uint64_t buffer_first = 0; //primer elemento del buffer


extern uint16_t pressedKeyShiftMap[][2];
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

static uint8_t handlekey(uint8_t key){
    uint8_t key_is_pressed = isPressed(key) ? 1:0;

    

    if(cntrlPressed && isPressed(key) && SAVE_REGS_KEY == pressedKeyShiftMap[][LOWER]) {
        inforegFlag = 1;
        return 0;
    }
    inforegFlag = 0;
    if(IS_CONTROL(key)) {
        cntrlPressed = (IS_PRESSED(key));
        return 0;
    }
    if(IS_SHIFT(key)) {
        map = (IS_PRESSED(key));
        return 0;
    }
    if(IS_BMAYUS(key) && IS_PRESSED(key)) {
        blockMayus = !blockMayus;
        return 0;
    }
    if(!isPressed(key) || pressedKeyShiftMap[map][key] == 0) return 0;  

    key = keyMapping[map][key];
    if(IS_ALPHA(key) && blockMayus) {
        key = (map == LOWERMAP)? TO_UPPER(key):TO_LOWER(key);
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
    return buffer_dim == 0
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
} */