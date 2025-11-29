#ifndef PROCESS_H
#define PROCESS_H

#include <limits.h>

#define PROC_NAME_SIZE 16

typedef enum proc_err_t : int {
	success,
	generic_error,
	open_file_failed,
	reading_failed,
} proc_err_t;

// Structure pour stocker les informations d'un seul processus
typedef struct Processus {
    char pid[NAME_MAX + 1];   // PID (Chaîne de caractères)
    char nom[PROC_NAME_SIZE];      // Nom du processus
    char etat;          // État du processus 
    
    long ram_rss;       // Mémoire utilisée
    char user[32];      // Nom de l'utilisateur
    unsigned long utime;    // Temps CPU utilisateur (14e valeur)
    unsigned long stime;    // Temps CPU système (15e valeur)
    float cpu_percent;
    
    struct Processus *suivant; // Pointeur vers le processus suivant (le chaînon)
} Processus;

bool proc_is_pid(const char *d_name);

proc_err_t proc_get_name(const char *pid, char *name);

#endif //PROCESS_H 
