#include <stdio.h>:
#include <dirent.h> //dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>


#include "process.h"

bool proc_is_pid(const char *filename) {
   	
	if(filename == nullptr || *filename == '\0') return false;

	while (*filename) {
		if(!isdigit((unsigned char)(*filename))) {
			return false;
		}
		++filename;
	}
	return true;
}

proc_err_t proc_get_name(const char *pid, char *name) {
	
	char path[PATH_MAX];
    
	snprintf(path, sizeof(path), "/proc/%s/comm", pid); 

	FILE *f = fopen(path, "r");

	if (f == nullptr) return proc_err_t::open_file_failed;

        if (fgets(name, PROC_NAME_SIZE, f) == nullptr) {
		fclose(f);
		return proc_err_t::reading_failed;
	}
	
	const int end_of_str = strcspn(name, "\n");
	name[end_of_str] = '\0';

        fclose(f);

	return proc_err_t::success;
}

// Fonction pour lire l'état du processus à partir de /proc/(PID)/stat
char lire_etat_processus(const char *pid) {
    char etat_proc[260];
    char etat = '?'; 

    // snprintf sert a stocker dans etat_proc le chemin du fichier stat pour le PID donné
    snprintf(etat_proc, sizeof(etat_proc), "/proc/%s/stat", pid); 
    
    FILE *f_etats = fopen(etat_proc, "r"); // ouvre /proc/<PID>/stat issue du snprinf

    if (f_etats != NULL) { 
        
        char buffer[1024];
        int pid_proc;
        
       
        if (fgets(buffer, sizeof(buffer), f_etats)) { // sert à lire la ligne du fichier stat en  verifiant que c'est possible
            // sscanf : récupérer juste le pid et l'état du processus sans le nom car on l'a deja et les stocker dans pid_proc et etat
            sscanf(buffer, "%d (%*[^)]) %c", &pid_proc, &etat); 
        }
        fclose(f_etats);
    }
    return etat;
}

void lire_utilisateur(const char *pid, char *tab_user,size_t tab_size){
    char chemin_user[300];
    char user[300]; 
    

    snprintf(chemin_user,sizeof(chemin_user),"/proc/%s/status", pid);

    FILE *f_user = fopen(chemin_user,"r");

    uid_t uid_valeur = -1;
    if (f_user != NULL) {
        while (fgets(user, sizeof(user), f_user) != NULL) {
            if (strncmp(user, "Uid:", 4) == 0) {
                sscanf(user, "%*s %u", &uid_valeur);  // recuperer la valeur de uid et la stocker dans uid_valeur
                break;
            }
        }
        fclose(f_user);  
    } else {
        snprintf(tab_user, tab_size, "fichier non accessible");
        return;
    }


    struct passwd *user_info = getpwuid(uid_valeur);  
    if (user_info != NULL){
        strncpy(tab_user,user_info->pw_name,tab_size);
    }
    else{
        snprintf(tab_user,tab_size,"%u", (unsigned)uid_valeur);
    }
}

    
long lire_memoire_rss(const char *pid){
    char memory[300];
    char ligne[300];
    long rss_pages = 0; //  la 6e valeur (en pages)
    long rss_octets=0;//  le résultat final


    snprintf(memory,sizeof(memory),"/proc/%s/statm", pid);

    FILE *f_memory = fopen(memory, "r");

    if (f_memory !=NULL){
        if(fgets(ligne, sizeof(ligne), f_memory)){

            sscanf(ligne,"%*lu %*lu %*lu %*lu %*lu %lu", &rss_pages );
            long taille_page = sysconf(_SC_PAGESIZE);
            rss_octets = rss_pages * taille_page; 
        }
        fclose(f_memory);
    }
    return rss_octets;
}
    


void lire_temps_cpu_proc(const char *pid, unsigned long *utime, unsigned long *stime){


}


Processus* ajouter_processus(Processus *tete_liste, const char *pid_proc, const char *nom_proc, char etat, const char *user_proc, long ram_rss, unsigned long utime, unsigned long stime, float cpu_percent)) {
    Processus *nouveau_proc = (Processus*)malloc(sizeof(Processus));

    if (nouveau_proc == NULL) {
        perror("Erreur d'allocation mémoire");
        return tete_liste; // Retourner la liste inchangée en cas d'erreur
    }
    strncpy(nouveau_proc->pid, pid_proc, sizeof(nouveau_proc->pid)); // copier le PID dans la structure
    strncpy(nouveau_proc->nom, nom_proc, sizeof(nouveau_proc->nom)); // copier le nom dans la structure
    nouveau_proc->etat = etat; // copier l'état dans la structure
    strncpy(nouveau_proc->user, user_proc, sizeof(nouveau_proc->user)); // copier l'état dans la structure
    nouveau_proc->ram_rss = ram_rss; // copier la ram dans la structure
    nouveau_proc->utime = utime;     // copier l'utime dans la structure
    nouveau_proc->stime = stime;     // copier le stime dans la structure
    nouveau_proc->cpu_pc = cpu_pc; // copier pourcentage du cpu dans la structure


    nouveau_proc->suivant = tete_liste; // le nouveau maillon pointe vers l'ancienne tête de liste
    return nouveau_proc; 
    }









Processus*liberer_liste(Processus *tete_liste) {
    Processus *courant = tete_liste;
    Processus *temp;

    while (courant != NULL) {
        temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    return NULL; 
} 






int main() {
    DIR *rep_proc = opendir("/proc");
    struct dirent *ent;  //permet d'utiliser les elements de la strcture dirent afin d'accéder au nom, type, etc..
    Processus *liste_processus = NULL;


    if (rep_proc == NULL) {
        perror("Erreur d'ouverture de /proc");
        return 1;
    }

    printf("%-8s | %-30s | %-5s\n", "PID", "Nom", "Etat");

    while ((ent = readdir(rep_proc)) != NULL) {

        if (est_un_pid(ent->d_name)) {         // Vérifier que d_name commence par un chiffre afin de faire un premier trie 


            char nom_proc[260]; // stockage du nom
            char etat = '?'; // au cas où l'état n'est pas trouvé
            char user_proc[32]; // stocker le nom d'utilisateur
            long ram_proc = 0;

            lire_nom_processus(ent->d_name, nom_proc, sizeof(nom_proc));
            etat = lire_etat_processus(ent->d_name);
            lire_utilisateur(ent->d_name, user_proc, sizeof(user_proc));
            ram_proc = lire_memoire_rss(ent->d_name);

liste_processus = ajouter_processus(liste_processus, ent->d_name, nom_proc, etat,user_proc, ram_proc,  0, 0, 0.0f // pas encore fait le cpu donc on met 0
            );
            lire_utilisateur(ent->d_name, user_proc,sizeof(user_proc));


        }
    } 

    Processus *courant = liste_processus;

    while (courant != NULL) {
        printf("%-8s | %-20s | %-6c | %-15s | %-10ld\n", 
            courant->pid, 
            courant->nom, 
            courant->etat,
            courant->user, 
            courant->ram_rss / 1024 
        );
        courant = courant->suivant;
    }
    
    liste_processus = liberer_liste(liste_processus);

    closedir(rep_proc);
    return 0;
}
