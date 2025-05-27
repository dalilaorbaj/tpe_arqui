#include <stdint.h>
#include <naiveConsole.h>
#include <keyboard.h>


extern void ncPrint(const char* string);
extern void ncNewline();
//Implementar
extern char ncGetChar();
extern void ncPrintChar(char c);

#define BUFFER_SIZE 100
#define COMMAND_MAX_LENGTH 10
#define ARGS_MAX_LENGTH 90

#define COMMAND_HELP "help"
#define COMMAND_CLEAR "clear"
#define COMMAND_ECHO "echo"
#define COMMAND_EXIT "exit"

// Función para leer una línea de entrada
static void readLine(char* buffer) {
    int index = 0;
    char c;
    
    while((c = ncGetChar()) != '\n' && index < BUFFER_SIZE - 1) {
        if(c == '\b' && index > 0) {
            // Manejo de backspace
            index--;
            ncPrint("\b \b"); // Borramos el caracter de la pantalla
        } else if(c >= ' ' && c <= '~') { // Caracteres imprimibles
            buffer[index++] = c;
            ncPrintChar(c); // Echo del caracter
        }
    }
    
    buffer[index] = '\0';
    ncNewline();
}

// Función para parsear el comando y argumentos
static void parseCommand(const char* buffer, char* command, char* args) {
    int i = 0, j = 0;
    
    // Extraer el comando
    while(buffer[i] != ' ' && buffer[i] != '\0' && i < COMMAND_MAX_LENGTH - 1) {
        command[i] = buffer[i];
        i++;
    }
    command[i] = '\0';
    
    // Saltar espacios
    while(buffer[i] == ' ') {
        i++;
    }
    
    // Extraer argumentos
    while(buffer[i] != '\0' && j < ARGS_MAX_LENGTH - 1) {
        args[j++] = buffer[i++];
    }
    args[j] = '\0';
}

// Comandos del shell
static void helpCommand() {
    ncPrint("Available commands:\n");
    ncPrint("  help - Display this help message\n");
    ncPrint("  clear - Clear the screen\n");
    ncPrint("  echo [text] - Display the provided text\n");
    ncPrint("  exit - Exit the shell\n");
}

static void clearCommand() {
    // Implementar limpieza de pantalla
    // Por ahora, solo imprimimos varias líneas nuevas
    for(int i = 0; i < 25; i++) {
        ncNewline();
    }
}

static void echoCommand(const char* args) {
    ncPrint(args);
    ncNewline();
}

// Función principal del shell
static void shellLoop() {
    char buffer[BUFFER_SIZE];
    char command[COMMAND_MAX_LENGTH];
    char args[ARGS_MAX_LENGTH];
    int running = 1;
    
    ncPrint("Welcome to the Simple Shell\n");
    ncPrint("Type 'help' for a list of commands\n");
    
    while(running) {
        ncPrint("$ ");
        readLine(buffer);
        
        // Parsear el comando y argumentos
        parseCommand(buffer, command, args);
        
        // Ejecutar el comando
        if(strcmp(command, COMMAND_HELP) == 0) {
            helpCommand();
        } else if(strcmp(command, COMMAND_CLEAR) == 0) {
            clearCommand();
        } else if(strcmp(command, COMMAND_ECHO) == 0) {
            echoCommand(args);
        } else if(strcmp(command, COMMAND_EXIT) == 0) {
            running = 0;
        } else if(command[0] != '\0') {
            ncPrint("Command not found: ");
            ncPrint(command);
            ncNewline();
        }
    }
}

// Función de comparación de strings (porque no podemos usar string.h)
static int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Punto de entrada del módulo
int main() {
    shellLoop();
    return 0;
}