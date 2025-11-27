#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

//dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 

int main() {
    DIR *rep_proc = opendir("/proc");
    struct dirent *ent; //permet d'utiliser les elements de la strcture dirent afin d'accéder au nom, type, etc..

    if (rep_proc == NULL) {
        perror("Erreur d'ouverture de /proc");
        return 1;
    }

    while ((ent = readdir(rep_proc)) != NULL) {

        // Vérifier que d_name commence par un chiffre afin de faire un premier trie 
        if (isdigit(ent->d_name[0])) {
            int i = 0;

            // Vérifier que tout le nom est composé de chiffres avec isdigit (outil venant de ctype.h) 
            while (ent->d_name[i] != '\0' && isdigit(ent->d_name[i])) {
                i++;
            }

            // Si on arrive à la fin de la chaîne alors on a trouvé le PID 
            if (ent->d_name[i] == '\0') {
                char dossier[300];

                snprintf(dossier, sizeof(dossier), "/proc/%s/comm",ent->d_name); // stocker dans dossier le nom correspondant a chaque PID  
                FILE *f = fopen(dossier, "r");

                if (f != NULL) { //verifier si le fichier il est bien ouvert 
                    char nom_proc[300];  //stockage des noms
                    fgets(nom_proc, sizeof(nom_proc),f); // récuperer le nom dans le fichier proc/pid/comm et le stocker dans nom_proc
                    printf("PID : %s | Nom : %s ", ent->d_name, nom_proc);
                    fclose(f);
                }
                char etats_proc[300];
                snprintf(etats_proc,sizeof(etats_proc),"/proc/%s/stat",ent->d_name); // pareil que pour le nom mais pour l'etat
                
                FILE *f_etats = fopen(etats_proc,"r"); // ouvre /proc/<PID>/stat issus du snprinf
 
                if(f_etats != NULL){
                    char buffer[1024];
                    int pid;
                    char etat;
                    fgets(buffer,sizeof(buffer),f_etats); // lire la ligne du fichier stat
                    sscanf(buffer, "%d (%*[^)]) %c", &pid, &etat);
                    printf(" Etat du processus : %c \n", etat);
                    fclose(f_etats);
                } 
            }
        }
    } // penser a regrouper les deux printf pour renvoyer tout d'un coup
    closedir(rep_proc);
    return 0;
}
