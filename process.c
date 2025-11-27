#include <stdio.h>
#include <dirent.h> //dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 
#include <ctype.h>
#include <string.h> 

// Fonction pour vérifier si le nom du répertoire est un PID (composé uniquement de chiffres)
int est_un_pid(const char *d_name) {
    if (!isdigit(d_name[0])) {
        return 0; 
    }

    int i = 0;
    while (d_name[i] != '\0' && isdigit(d_name[i])) {
        i++;
    }

    return (d_name[i] == '\0');
}


// Fonction pour lire le nom du processus à partir de /proc/<PID>/comm
void lire_nom_processus(const char *pid_proc, char *nom_proc_tab, size_t stock_taille) {
    char chemin[300];
    
    // snprintf : stocker dans chemin le chemin du fichier comm pour le PID donné
    snprintf(chemin, sizeof(chemin), "/proc/%s/comm", pid_proc); 

    FILE *f = fopen(chemin, "r");

    if (f != NULL) { // vérifier si le fichier est bien ouvert
        // fgets : récupérer le nom et le stocker dans nom_proc_tab
        if (fgets(nom_proc_tab, stock_taille, f) != NULL) {

            nom_proc_tab[strcspn(nom_proc_tab, "\n")] = 0; // faciliter l'affichage pour Matis 

        } else {
            snprintf(nom_proc_tab, stock_taille, "probleme de lecture");
        }
        fclose(f);

    } else {
        // En cas d'échec d'ouverture du fichier
        snprintf(nom_proc_tab, stock_taille, "fichier non accessible");
    }
}

// Fonction pour lire l'état du processus à partir de /proc/(PID)/stat
char lire_etat_processus(const char *pid) {
    char etat_proc[260];
    char etat = '?'; 

    // snprintf sert a stocker dans chemin_stat le chemin du fichier stat pour le PID donné
    snprintf(etat_proc, sizeof(etat_proc), "/proc/%s/stat", pid); 
    
    FILE *f_etats = fopen(etat_proc, "r"); // ouvre /proc/<PID>/stat issue du snprinf

    if (f_etats != NULL) { //verifier si le fichier il est bien ouvert 
        
        char buffer[1024];
        int pid_proc;
        
       
        if (fgets(buffer, sizeof(buffer), f_etats)) { // sert à lire la ligne du fichier stat verifiant que c'est possible
            // sscanf : récupérer juste le pid et l'état du processus sans le nom car on l'a deja et les stocker dans pid_proc et etat
            sscanf(buffer, "%d (%*[^)]) %c", &pid_proc, &etat); 
        }
        fclose(f_etats);
    }
    return etat;
}

int main() {
    DIR *rep_proc = opendir("/proc");
    struct dirent *ent;  //permet d'utiliser les elements de la strcture dirent afin d'accéder au nom, type, etc..


    if (rep_proc == NULL) {
        perror("Erreur d'ouverture de /proc");
        return 1;
    }

    printf("%-8s | %-30s | %-5s\n", "PID", "Nom", "Etat");

    while ((ent = readdir(rep_proc)) != NULL) {

        if (est_un_pid(ent->d_name)) {         // Vérifier que d_name commence par un chiffre afin de faire un premier trie 


            char nom_proc[260]; // stockage du nom
            char etat = '?'; // au cas où l'état n'est pas trouvé

            lire_nom_processus(ent->d_name, nom_proc, sizeof(nom_proc));

            etat = lire_etat_processus(ent->d_name);

            printf("%-8s | %-30s | %-5c\n", ent->d_name, nom_proc, etat);
        }
    } 

    closedir(rep_proc);
    return 0;
}
