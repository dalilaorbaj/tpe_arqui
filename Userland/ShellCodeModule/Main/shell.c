#include <stdint.h>
#include "keyboard.h"
#include "naiveConsole.h"
#include <library.h>
#include "regs_snapshot.h"



extern void ncPrint(const char* string);
extern void ncNewline();
//Implementar
extern char ncGetChar();
extern void ncPrintChar(char c);
static void shellLoop();
static void regsCommand();
extern void invOpcode();

int timeCommand();
void divZeroCommand();
void invOpcodeCommand();
int sys_get_registers(RegsSnapshot *regs);
void printRegsSnapshot(const RegsSnapshot *regs);
int64_t writeStr(const char *s);

#define SHELL_BUFFER_SIZE 100
#define COMMAND_MAX_LENGTH 10
#define ARGS_MAX_LENGTH 90

#define COMMAND_HELP "help"
#define COMMAND_CLEAR "clear"
#define COMMAND_ECHO "echo"
#define COMMAND_EXIT "exit"
#define COMMAND_TIME "time"
#define COMMAND_DIVZERO "divzero"
#define COMMAND_INVOPCODE "invopcode"
#define COMMAND_REGS "regs"

// Punto de entrada del módulo
int main() {
    puts("Shell code module loaded successfully!");
    //ncPrint("SHELL MAIN!\n");
    shellLoop();
    return 0;
}

// Función para leer una línea de entrada
static void readLine(char* buffer, int buffer_size) {
    int index = 0;
    char * c;
    
    while((*c = ncGetChar()) != '\n' && index < buffer_size - 1) {
        if(*c == '\b' && index > 0) {
            // Manejo de backspace
            index--;
            puts("\b \b"); // Borramos el caracter de la pantalla
        } else if(*c >= ' ' && *c <= '~' && index < buffer_size - 1) {
            buffer[index++] = *c;
            puts((const char *) c); // Echo del caracter
        }
    }
    
    buffer[index] = '\0';
    ncNewline();
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
    puts("  help - Display this help message\n");
    puts("  clear - Clear the screen\n");
    puts("  echo [text] - Display the provided text\n");
    puts("  exit - Exit the shell\n");
}

static void clearCommand() {
    clearScreen();
}

static void echoCommand(const char* args) {
    puts(args);
    ncNewline();
}

// Función principal del shell
static void shellLoop() {
    char buffer[SHELL_BUFFER_SIZE];
    char command[COMMAND_MAX_LENGTH];
    char args[ARGS_MAX_LENGTH];
    int running = 1;

    puts(">"); // Inicializar el cursor
    
    puts("Welcome to the Simple Shell");
    puts("Type 'help' for a list of commands");
    
    while(running) {
        puts("$ ");
        readLine(buffer, SHELL_BUFFER_SIZE);
        
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
            ncPrint(command);
            ncNewline();
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

//(?) las funciones de abajo deberian ser static?


void divZeroCommand() {
    volatile int a = 1, b = 0;
    a = a / b;
}

void invOpcodeCommand() {
    invOpcode();
}


// ya esta definida en library.c asi que la comento aca
// static int strcmp(const char* s1, const char* s2) {
//     while(*s1 && (*s1 == *s2)) {
//         s1++;
//         s2++;
//     }
//     return *(const unsigned char*)s1 - *(const unsigned char*)s2;
// }



