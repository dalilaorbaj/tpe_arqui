#include <stdint.h>
//#include "regs_snapshot.h"
#include <shell.h>

static void shellLoop();
static void regsCommand();
extern void invOpcode();
static void helpCommand();
static void echoCommand();
static void timeCommand();
static void divZeroCommand();
static void beepCommand();
static void invOpcodeCommand();
static void pongisCommand();
void printRegsSnapshot(const RegsSnapshot *regs);
int64_t writeStr(int fd, const char *s);
static void clearCommand();
static void exitCommand();
static void utcToMinusUtc3(time_struct *time);
static void clearKeyboardBuffer();


static Option options[] = {
    {"help", helpCommand},
    {"clear", clearCommand},
    {"echo", echoCommand},
    {"exit", exitCommand},
    {"time", timeCommand},
    {"divzero", divZeroCommand},
    {"invopcode", invOpcodeCommand},
    {"regs", regsCommand},
    {"beep", beepCommand}, 
    {"pongis", pongisCommand}
};

// Punto de entrada del módulo
int main() {
    puts("Se cargó la shell correctamente!");
    shellLoop();
    return 0;
}


// Función para parsear el comando y argumentos
static void parseCommand(const char* buffer, char* command, int command_size, char* args, int args_size) {
    int i = 0, j = 0;
    // Extraer el comando
    while(buffer[i] != ' ' && buffer[i] != '\0' && i < command_size - 1) {
        command[i] = buffer[i];
        i++;
    }
    command[i] = '\0';

    // Saltar espacios
    while(buffer[i] == ' ') i++;

    // Extraer argumentos
    j = 0;
    while(buffer[i] != '\0' && j < args_size - 1) {
        args[j++] = buffer[i++];
    }
    args[j] = '\0';
}

// Comandos del shell
static void helpCommand() {
    writeStrColor("Available commands:\n", (Color){129, 201, 255});
    writeStrColor("  help ", (Color){129, 243, 255});
    puts("- Display this help message");
    writeStrColor("  clear ", (Color){129, 243, 255});
    puts("- Clear the screen");
    writeStrColor("  echo [text] ", (Color){129, 243, 255});
    puts("- Display the provided text");
    writeStrColor("  time ", (Color){129, 243, 255});
    puts("- Show current date and time");
    writeStrColor("  regs ", (Color){129, 243, 255});
    puts("- Show CPU registers");
    writeStrColor("  divzero ", (Color){129, 243, 255});
    puts("- Test division by zero exception");
    writeStrColor("  invopcode ", (Color){129, 243, 255});
    puts("- Test invalid opcode exception");
    writeStrColor("  exit ", (Color){129, 243, 255});
    puts("- Exit the shell"); //chequear si se puede salir de la shell
    writeStrColor("  beep ", (Color){129, 243, 255});
    puts("- Make a sound");
    writeStrColor("  pongis ", (Color){129, 243, 255});
    puts("- Play Pongis Golf");

}

static void clearCommand() {
    clearScreen();
}

static void beepCommand() {
    beep(440, 18);
}

static void echoCommand(const char* args) {
    puts(args);
    puts("");
}

// Función para limpiar el buffer del teclado
static void clearKeyboardBuffer() {
    uint16_t c;
    int attempts = 0;
    const int MAX_ATTEMPTS = 1000;  // Evitar bucle infinito
    
    // Leer todas las teclas pendientes
    while(attempts < MAX_ATTEMPTS) {
        int result = sys_read(0, &c, 1);
        if(result <= 0) {
            break;  // No hay más teclas
        }
        attempts++;
    }
}

// Función principal del shell
static void shellLoop() {
    char buffer[SHELL_BUFFER_SIZE];
    char command[COMMAND_MAX_LENGTH];
    char args[ARGS_MAX_LENGTH];
    int running = 1;

    clearScreen();
    
    writeStrColor("====Welcome to the Simple Shell====\n", (Color){158, 255, 220});
    timeCommand();  // Mostrar hora al iniciar

    puts("");
    writeStrColor("Type 'help' for a list of commands\n", (Color){158, 255, 220});
    
    int found;
    while(running) {
        puts("");  
        writeStrColor("$> ", (Color){255, 158, 161}); // Prompt
        gets(buffer, SHELL_BUFFER_SIZE);  // Usar readLine en lugar de gets
        
        if(strlen(buffer) == 0) {
            continue;
        }
        
        // Parsear comando para manejar argumentos
        parseCommand(buffer, command, COMMAND_MAX_LENGTH, args, ARGS_MAX_LENGTH);
        
        found = 0;
        for(int i = 0; i < CANT_OPTIONS; i++) {
            if(strcmp(command, options[i].name) == 0) {
                if(strcmp(command, "exit") == 0) {
                    running = 0;
                } else if(strcmp(command, "echo") == 0) {
                    echoCommand(args);  // Pasar argumentos
                } else {
                    options[i].function();
                }
                found = 1;
                break;
            }
        }
        if(!found){
            puts("Comando no reconocido");
        }
        
    }
}

static void regsCommand() {
    RegsSnapshot regs;
    if (sys_get_registers(&regs) == -1) {
        puts("Error al obtener los registros");
    } else {
        printRegsSnapshot(&regs);
    }
}


static void divZeroCommand() {
    volatile int a = 1, b = 0;
    a = a / b;
}

static void invOpcodeCommand() {
    invOpcode();
}

static void timeCommand(){
    time_struct actualTime;
    sys_get_time(&actualTime);
    utcToMinusUtc3(&actualTime); // Convertir de UTC a UTC-3 (Argentina)
    puts("Hora actual (UTC-3):");
    printf("%d/%d/%d [d/m/y]\n", actualTime.day, actualTime.month, actualTime.year);
    printf("%d:%d:%d [hour/min/sec] (Argentina)\n", actualTime.hour, actualTime.minutes, actualTime.seconds); 
}

static void pongisCommand(){
    startPongisGolf();
    return;
}

// Convertir UTC a UTC-3 
static void utcToMinusUtc3(time_struct *time) {
    // le quitamos 3 horas a la hora UTC
    if (time->hour < 3) {
        // Si la hora es menor a 3, debemos restar un día y ajustar la hora
        time->hour = time->hour + 24 - 3;
        
        time->day--;
        
        // Chequeamos si hay que ajustar el mes y el año
        if (time->day == 0) {
            time->month--;
            
            if (time->month == 0) {
                time->month = 12;
                time->year--;
            }
            
            switch (time->month) {
                case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    time->day = 31;
                    break;
                case 4: case 6: case 9: case 11:
                    time->day = 30;
                    break;
                case 2:
                    // chqueamos si es año bisiesto
                    if ((time->year % 4 == 0 && time->year % 100 != 0) || (time->year % 400 == 0))
                        time->day = 29;
                    else
                        time->day = 28;
                    break;
            }
        }
    } else {
        // Si la hora es mayor o igual a 3, simplemente restamos 3 horas
        time->hour -= 3;
    }
}

static void exitCommand() {
    puts("Abandonando la shell...\n");
    return ;
}





