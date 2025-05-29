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
static void invOpcodeCommand();
void printRegsSnapshot(const RegsSnapshot *regs);
int64_t writeStr(int fd, const char *s);
static void clearCommand();
static void exitCommand();

#define COMMAND_HELP "help"
#define COMMAND_CLEAR "clear"
#define COMMAND_ECHO "echo"
#define COMMAND_EXIT "exit"
#define COMMAND_TIME "time"
#define COMMAND_DIVZERO "divzero"
#define COMMAND_INVOPCODE "invopcode"
#define COMMAND_REGS "regs"

static Option options[] = {
    {"help", helpCommand},
    {"clear", clearCommand},
    {"echo", echoCommand},
    {"exit", exitCommand},
    {"time", timeCommand},
    {"divzero", divZeroCommand},
    {"invopcode", invOpcodeCommand},
    {"regs", regsCommand}
};

// Punto de entrada del módulo
int main() {
    puts("Shell code module loaded successfully!");
    puts("Initializing shellLoop...");
    shellLoop();
    return 0;
}

// Función para leer una línea de entrada
static void readLine(char* buffer, int buffer_size) {
    int index = 0;
    char c;  
    
    while(index < buffer_size - 1) {
        // Usar syscall sys_read para leer un caracter desde STDIN (fd = 0)
        if(sys_read(0, (uint16_t *)&c, 1) > 0) {
            if(c == '\n' || c == '\r') {
                break; // Terminar la línea
            } else if(c == '\b' && index > 0) {
                // Manejo de backspace
                index--;
                puts("\b \b"); // Borramos el caracter de la pantalla
            } else if(c >= ' ' && c <= '~') {
                buffer[index++] = c;
                // Para imprimir un caracter, crea un string
                char temp[2] = {c, '\0'};
                puts(temp); // Echo del caracter
            }
        }
    }
    
    buffer[index] = '\0';
    puts(""); // Salto de línea
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
    puts("  echo [text] - Display the provided text\n");
    puts("  time - Show current date and time\n");
    puts("  regs - Show CPU registers\n");
    puts("  divzero - Test division by zero exception\n");
    puts("  invopcode - Test invalid opcode exception\n");
    puts("  exit - Exit the shell\n");
}

static void clearCommand() {
    clearScreen();
}

static void echoCommand(const char* args) {
    puts(args);
    puts("");
}

static void testKeyboard() {
    puts("Presiona una tecla (test directo):\n");
    uint16_t c;
    
    // Test directo de sys_read
    while(1) {
        int result = sys_read(0, &c, 1);
        if(result > 0) {
            puts("Tecla detectada: ");
            char temp[2] = {(char)c, '\0'};
            puts(temp);
            puts("\n");
            break;
        }
        // Pequeña pausa para no saturar
        for(int i = 0; i < 1000000; i++);
    }
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
    testKeyboard();
    
    // Limpiar buffer después del test
    clearKeyboardBuffer();
    
    puts("Welcome to the Simple Shell");
    puts("Type 'help' for a list of commands");
    
    int found;
    while(running) {
        puts("$ ");
        readLine(buffer, SHELL_BUFFER_SIZE);  // Usar readLine en lugar de gets
        
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
        
        //readLine(buffer, SHELL_BUFFER_SIZE);
        /*
        // Parsear el comando y argumentos
        parseCommand(buffer, command, COMMAND_MAX_LENGTH, args, ARGS_MAX_LENGTH);
        
        // Ejecutar el comando
        if(strcmp(command, COMMAND_HELP) == 0) {
            helpCommand();
        } else if(strcmp(command, COMMAND_CLEAR) == 0) {
            clearCommand();
        } else if(strcmp(command, COMMAND_ECHO) == 0) {
            echoCommand(args);
        } else if(strcmp(command, COMMAND_TIME) == 0) {
            timeCommand();
        } else if(strcmp(command, COMMAND_DIVZERO) == 0) {
            divZeroCommand();
        } else if(strcmp(command, COMMAND_INVOPCODE) == 0) {
            invOpcodeCommand();
        } else if(strcmp(command, COMMAND_REGS) == 0) {
            regsCommand();
        } else if(strcmp(command, COMMAND_EXIT) == 0) {
            running = 0;
        } else if (command[0] != '\0'){
            puts("Comando no encontrado: ");
            puts(command);
            puts("");
        } */
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

//(?) las funciones de abajo deberian ser static?


static void divZeroCommand() {
    volatile int a = 1, b = 0;
    a = a / b;
}

static void invOpcodeCommand() {
    invOpcode();
}

// static void clearCommand(){
//     clearScreen();
// }

static void timeCommand(){
    time_struct actualTime;
    sys_get_time(&actualTime);
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
    puts("Exiting shell...\n");
    
}

static void echoComand() {
    
}


// ya esta definida en library.c asi que la comento aca
// static int strcmp(const char* s1, const char* s2) {
//     while(*s1 && (*s1 == *s2)) {
//         s1++;
//         s2++;
//     }
//     return *(const unsigned char*)s1 - *(const unsigned char*)s2;
// }



