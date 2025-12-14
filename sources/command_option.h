#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H

#define MAX_LENGTH 256

#include <stdbool.h>

typedef struct {
    bool help;
    bool dry_run;
    char remote_config[MAX_LENGTH];    
    char connexion_type[MAX_LENGTH];
    int port;
    char login[MAX_LENGTH]; 
    char remote_server[MAX_LENGTH]; 
    char username[MAX_LENGTH];
    char password[MAX_LENGTH]; 
    bool all;

} options_prog; 

typedef struct {
    char name_server[MAX_LENGTH];
    char adress_server[MAX_LENGTH];
    int port; 
    char username[MAX_LENGTH];
    char password[MAX_LENGTH]; 
    char type_co[7];
} fichier;

int command_run(int argc, char *argv[],options_prog *options);

#endif 
