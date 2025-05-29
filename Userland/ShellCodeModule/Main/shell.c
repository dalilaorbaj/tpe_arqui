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
void printRegsSnapshot(const RegsSnapshot *regs);
int64_t writeStr(int fd, const char *s);
static void clearCommand();
static void exitCommand();
static void utcToUtc3(time_struct *time);
static void clearKeyboardBuffer();

// #define COMMAND_HELP "help"
// #define COMMAND_CLEAR "clear"
// #define COMMAND_ECHO "echo"
// #define COMMAND_EXIT "exit"
// #define COMMAND_TIME "time"
// #define COMMAND_DIVZERO "divzero"
// #define COMMAND_INVOPCODE "invopcode"
// #define COMMAND_REGS "regs"

static Option options[] = {
    {"help", helpCommand},
    {"clear", clearCommand},
    {"echo", echoCommand},
    {"exit", exitCommand},
    {"time", timeCommand},
    {"divzero", divZeroCommand},
    {"invopcode", invOpcodeCommand},
    {"regs", regsCommand},
    {"beep", beepCommand}
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
    puts("Available commands:");
    puts("  help - Display this help message");
    puts("  clear - Clear the screen");
    puts("  echo [text] - Display the provided text");
    puts("  time - Show current date and time");
    puts("  regs - Show CPU registers");
    puts("  divzero - Test division by zero exception");
    puts("  invopcode - Test invalid opcode exception");
    puts("  exit - Exit the shell");
    puts("  beep - Make a sound");
}

static void clearCommand() {
    clearScreen();
}

static void beepCommand() {
    beep(440, 10000);
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
    
    puts("Welcome to the Simple Shell");
    puts("Type 'help' for a list of commands");
    
    int found;
    while(running) {
        puts("");
        putChar('>');  // Prompt
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
    if (sys_get_registers(&regs) == 0) {
        printRegsSnapshot(&regs);
    } else {
        puts("Error al obtener los registros");
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
    utcToUtc3(&actualTime); // Convertir de UTC a UTC-3 (Argentina)
    puts("Hora actual (UTC-3):");
    printf("%d/%d/%d [d/m/y]\n", actualTime.day, actualTime.month, actualTime.year);
    printf("%d:%d:%d [hour/min/sec] (Argentina)\n", actualTime.hour, actualTime.minutes, actualTime.seconds); 
}

// Convertir UTC a UTC-3 
static void utcToUtc3(time_struct *time) {
    // Subtract 3 hours from the time
    if (time->hour < 3) {
        // Need to go back one day
        time->hour = time->hour + 24 - 3;
        
        // Handle day rollover
        time->day--;
        
        // Check for month rollover
        if (time->day == 0) {
            time->month--;
            
            // Handle month rollover
            if (time->month == 0) {
                time->month = 12;
                time->year--;
            }
            
            // Set the day to the last day of the previous month
            switch (time->month) {
                case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    time->day = 31;
                    break;
                case 4: case 6: case 9: case 11:
                    time->day = 30;
                    break;
                case 2:
                    // Leap year check
                    if ((time->year % 4 == 0 && time->year % 100 != 0) || (time->year % 400 == 0))
                        time->day = 29;
                    else
                        time->day = 28;
                    break;
            }
        }
    } else {
        // Simple case - just subtract 3 hour
        time->hour -= 3;
    }
}

static void exitCommand() {
    puts("Abandonando la shell...\n");
    return 0;
}





