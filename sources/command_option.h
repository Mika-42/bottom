#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H
#include <stdbool.h>

int command_run(int argc, char *argv[]);

typedef struct 
{
    bool help
    bool dry_run
    *char remote_config
    *char connexion_type
    int port
    *char login
    *char remote_server


} options_t

#endif //COMMAND_OPTION_H
