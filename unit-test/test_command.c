#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H
#define MAX_LENGTH 255
#include <stdbool.h>

typedef struct 
{
    bool help;
    bool dry_run;
    // Changements : Passage de 'char *name[MAX_LENGTH]' à 'char name[MAX_LENGTH]' 
    // pour stocker la chaîne directement.
    char remote_config[MAX_LENGTH];    
    char connexion_type[MAX_LENGTH];
    int port;
    // Les options 'login', 'remote_server', 'username', 'password'
    // peuvent être laissées comme des pointeurs (char *) pour utiliser strdup/malloc, 
    // ou passées en tableaux fixes si vous décidez de copier la chaîne directement.
    // Je vais les laisser en 'char *' car c'est ce que votre code .c essayait de faire
    // avec les options getopt (strdup/optarg), mais on va corriger l'allocation/le type dans la structure.
    char *login; 
    char *remote_server; 
    char *username;
    char *password; 
    bool all;

} options_prog; 

typedef struct 
{
    // Changement : Passage de 'char *name[MAX_LENGTH]' à 'char name[MAX_LENGTH]'
    char name_server[MAX_LENGTH];
    char adress_server[MAX_LENGTH];
    int port; 
    char username[MAX_LENGTH];
    char password[MAX_LENGTH]; 
    // Changement : Correction de la taille pour type_co
    char type_co[7]; // Suffisant pour "telnet" ou "ssh"
} fichier;

int command_run(int argc, char *argv[],options_prog *options);

#endif