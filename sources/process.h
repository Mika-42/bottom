#ifndef PROCESS_H
#define PROCESS_H

// Structure pour stocker les informations d'un seul processus
typedef struct Processus {
    char pid[16];   // PID (Chaîne de caractères)
    char nom[260];      // Nom du processus
    char etat;          // État du processus 
    
    long ram_rss;       // Mémoire utilisée
    char user[32];      // Nom de l'utilisateur
    unsigned long utime;    // Temps CPU utilisateur (14e valeur)
    unsigned long stime;    // Temps CPU système (15e valeur)
    float cpu_percent;
    
    struct Processus *suivant; // Pointeur vers le processus suivant (le chaînon)
} Processus;

#endif //PROCESS_H 
