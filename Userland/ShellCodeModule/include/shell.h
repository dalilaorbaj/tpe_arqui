//#include <library.h>
#include <pongisGolf.h>

#define SHELL_BUFFER_SIZE 100
#define COMMAND_MAX_LENGTH 20
#define ARGS_MAX_LENGTH 90
//(!) mal estilo, despues cambiar esto!!!!
#define CANT_OPTIONS 16
#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct Option{
    char * name;
    void (* function)();
} Option;

