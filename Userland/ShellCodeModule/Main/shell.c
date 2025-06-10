#include <stdint.h>
#include <shell.h>

static void shellLoop();
static void regsCommand();
static void helpCommand();
static void echoCommand();
static void timeCommand();
static void beepCommand();
static void pongisCommand();
void printRegsSnapshot(const RegsSnapshot *regs);
int64_t writeStr(int fd, const char *s);
static void clearCommand();
static void zoomInCommand();
static void zoomOutCommand();
static void dyslexicModeCommand();
static void defaultModeCommand();
static void utcToMinusUtc3(time_struct *time);
// static void clearKeyboardBuffer();


static Option options[] = {
    {"help", helpCommand},
    {"clear", clearCommand},
    {"echo", echoCommand},
    {"time", timeCommand},
    {"divzero", divZero},
    {"invopcode", invOpcode},
    {"regs", regsCommand},
    {"beep", beepCommand}, 
    {"pongis", pongisCommand}, 
    {"zoomIn", zoomInCommand},
    {"zoomOut", zoomOutCommand},
    {"MK", play_mortal_kombat_theme},
    {"GOT", play_got_theme},
    {"cockroach", play_cockroach_song}, 
    {"dyslexicMode", dyslexicModeCommand}, 
    {"defaultMode", defaultModeCommand}
};

// Punto de entrada del módulo
int main() {
    clearScreen();
    shellLoop();
    return 0;
}


static void parseCommand(const char* buffer, char* command, int command_size, char* args, int args_size) {
    int i = 0, j = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0' && i < command_size - 1) {
        command[i] = buffer[i];
        i++;
    }
    command[i] = '\0';

    while(buffer[i] == ' ') i++;

    j = 0;
    while(buffer[i] != '\0' && j < args_size - 1) {
        args[j++] = buffer[i++];
    }
    args[j] = '\0';
}

// Comandos del shell
static void helpCommand() {
    writeStrColor("Available commands:\n", COLOR_ORANGE);
    writeStrColor("  help ", COLOR_YELLOW);
    puts("- Display this help message");
    writeStrColor("  clear ", COLOR_YELLOW);
    puts("- Clear the screen");
    writeStrColor("  zoomIn ", COLOR_YELLOW);
    puts("- Zoom in the screen");
    writeStrColor("  zoomOut ", COLOR_YELLOW);
    puts("- Zoom out the screen");
    writeStrColor("  echo [text] ", COLOR_YELLOW);
    puts("- Display the provided text");
    writeStrColor("  dyslexicMode ", COLOR_YELLOW);
    puts("- Switch to dyslexic mode (changes font)");
    writeStrColor("  defaultMode ", COLOR_YELLOW);
    puts("- Switch to default mode (changes font)");
    writeStrColor("  time ", COLOR_YELLOW);
    puts("- Show current date and time");
    writeStrColor("  regs ", COLOR_YELLOW);
    puts("- Show CPU registers. Remember to press F9 beforehand!!");
    writeStrColor("  divzero ", COLOR_YELLOW);
    puts("- Test division by zero exception");
    writeStrColor("  invopcode ", COLOR_YELLOW);
    puts("- Test invalid opcode exception");
    writeStrColor("  pongis ", COLOR_YELLOW);
    puts("- Play Pongis Golf");
    writeStrColor("  beep ", COLOR_YELLOW);
    puts("- Make a sound");
    writeStrColor("  MK ", COLOR_YELLOW);
    puts("- Plays the Mortal Kombat Theme");
    writeStrColor("  GOT ", COLOR_YELLOW);
    puts("- Plays the Game Of Thrones Theme");
    writeStrColor("  cockroach ", COLOR_YELLOW);
    puts("- Plays the Cockroach Song");
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

// static void clearKeyboardBuffer() {
//     uint16_t c;
//     int attempts = 0;
//     const int MAX_ATTEMPTS = 1000; 
//     while(attempts < MAX_ATTEMPTS) {
//         int result = sys_read(0, &c, 1);
//         if(result <= 0) {
//             break;
//         }
//         attempts++;
//     }
// }

// Función principal del shell
static void shellLoop() {
    char buffer[SHELL_BUFFER_SIZE];
    char command[COMMAND_MAX_LENGTH];
    char args[ARGS_MAX_LENGTH];
    int running = 1;

    clearScreen();
    
    writeStrColor("====Welcome to DyOS====\n", COLOR_MINT);
    timeCommand();

    puts("");
    writeStrColor("Type 'help' for a list of commands\n", COLOR_MINT);
    
    int found;
    while(running) {
        puts("");  
        writeStrColor("$> ", COLOR_PURPLE);
        gets(buffer, SHELL_BUFFER_SIZE); 
        
        if(strlen(buffer) == 0) {
            continue;
        }
        
        parseCommand(buffer, command, COMMAND_MAX_LENGTH, args, ARGS_MAX_LENGTH);
        
        found = 0;
        for(int i = 0; i < CANT_OPTIONS; i++) {
            if(strcmp(command, options[i].name) == 0) {
                if(strcmp(command, "echo") == 0) {
                    echoCommand(args);
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
    return;
}

static void regsCommand() {
    RegsSnapshot regs;
    if (sys_get_registers(&regs) == -1) {
        puts("Error al obtener los registros");
    } else {
        printRegsSnapshot(&regs);
    }
}

static void timeCommand(){
    time_struct actualTime;
    sys_get_time(&actualTime);
    utcToMinusUtc3(&actualTime); 
    writeStrColor("Current time (UTC-3): \n", COLOR_PURPLE);     
    printf("%d/%d/%d ", actualTime.day, actualTime.month, actualTime.year);
    writeStrColor("[d/m/y]\n", COLOR_YELLOW);   
    printf("%d:%d:%d ", actualTime.hour, actualTime.minutes, actualTime.seconds); 
    writeStrColor("[hour/min/sec] (Argentina)\n", COLOR_YELLOW);   
}

static void pongisCommand(){
    startPongisGolf();
    return;
}

static void utcToMinusUtc3(time_struct *time) {
    if (time->hour < 3) {
        time->hour = time->hour + 24 - 3;
        time->day--;
        
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
                    if ((time->year % 4 == 0 && time->year % 100 != 0) || (time->year % 400 == 0))
                        time->day = 29;
                    else
                        time->day = 28;
                    break;
            }
        }
    } else {
        time->hour -= 3;
    }
}

static void zoomInCommand() {
    zoomIn();
}

static void zoomOutCommand() {
    zoomOut();
}

static void dyslexicModeCommand() {
    dyslexicMode();
}

static void defaultModeCommand() {
    defaultMode();
}