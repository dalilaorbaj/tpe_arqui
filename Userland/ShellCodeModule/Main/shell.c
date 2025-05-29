#include <stdint.h>
#include <library.h>
//#include "regs_snapshot.h"
#include <shell.h>

static void shellLoop();
static void regsCommand();
extern void invOpcode();

static void timeCommand();
void divZeroCommand();
void invOpcodeCommand();
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

// Función principal del shell
static void shellLoop() {
    char buffer[SHELL_BUFFER_SIZE];
    char command[COMMAND_MAX_LENGTH];
    char args[ARGS_MAX_LENGTH];
    int running = 1;

    clearScreen();
    
    puts("Welcome to the Simple Shell");
    puts("Type 'help' for a list of commands");
    
    while(running) {
        puts("$ ");
        gets(buffer, COMMAND_MAX_LENGTH);
        if(strlen(buffer)==0){
            return ;
        }
        for(int i=0 ; i<CANT_OPTIONS ; i++){
            if(strcmp(buffer, options[i].name)==0){
                options[i].function();
                return;
            }
        }
        fprintf(STDERR, )
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


void divZeroCommand() {
    volatile int a = 1, b = 0;
    a = a / b;
}

void invOpcodeCommand() {
    invOpcode();
}

static void clearCommand(){
    clearScreen();
}

static void timeCommand(){
    time_struct actualTime;
    sys_get_time(&actualTime);
    
}

static void exitCommand() {
    puts("Exiting shell...\n");
    
}


// ya esta definida en library.c asi que la comento aca
// static int strcmp(const char* s1, const char* s2) {
//     while(*s1 && (*s1 == *s2)) {
//         s1++;
//         s2++;
//     }
//     return *(const unsigned char*)s1 - *(const unsigned char*)s2;
// }



