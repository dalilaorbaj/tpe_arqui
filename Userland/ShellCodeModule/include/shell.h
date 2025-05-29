#include <library.h>

#define SHELL_BUFFER_SIZE 100
#define COMMAND_MAX_LENGTH 10
#define ARGS_MAX_LENGTH 90
#define CANT_OPTIONS 8
#define STDERR -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct Option{
    char * name;
    void (* function)();
} Option;

