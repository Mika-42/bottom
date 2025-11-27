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
                printf("Le PID est : %s\n", ent->d_name);
            }
        }
    }
    closedir(rep_proc);
    return 0;
}
