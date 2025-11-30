#include <stdio.h>
#include <dirent.h> //dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>



// Structure pour stocker les informations d'un seul processus
typedef struct Processus {
    char pid[16];   // PID (Chaîne de caractères)
    char nom[260];      // Nom du processus
    char etat;          // État du processus 
    
    long ram_rss;       // Mémoire utilisée
    char user[32];      // Nom de l'utilisateur
    unsigned long utime;    // Temps CPU utilisateur (14e valeur)
    unsigned long stime;    // Temps CPU système (15e valeur)
    float cpu_pc;
    
    struct Processus *suivant; // Pointeur vers le processus suivant (le chaînon)
} Processus;


// Fonction pour vérifier si le nom du répertoire est un PID (composé uniquement de chiffres)
int est_un_pid(const char *d_name) {
    if (!isdigit(d_name[0])) {
        return 0; 
    }

    int i = 0;
    while (d_name[i] != '\0' && isdigit(d_name[i])) {  // conditon pour savoir si cest un PID
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

    // snprintf sert a stocker dans etat_proc le chemin du fichier stat pour le PID donné
    snprintf(etat_proc, sizeof(etat_proc), "/proc/%s/stat", pid); 
    
    FILE *f_etats = fopen(etat_proc, "r"); // ouvre /proc/<PID>/stat issue du snprinf

    if (f_etats != NULL) { //verifier si le fichier il est bien ouvert 
        
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
    
void proc_get_cpu_time(const char *pid, unsigned long *utime, unsigned long *stime){

    char line[2048];
    char buffer[2048];
    *utime=0;
    *stime=0;

    snprintf(line, sizeof(line),"/proc/%s/stat", pid);

    FILE *f_cpu = fopen(line, "r");

    if (f_cpu==NULL) return;

    if(fgets(buffer, sizeof(buffer), f_cpu)){

        char *p = strrchr(buffer, ')'); 
        
        if (p!= NULL){

            sscanf(p + 2, "%*c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu", utime, stime);
        }
    }
    fclose(f_cpu);
}


    
Processus* trouver_processus_par_pid(Processus *tete_liste, const char *pid) {
    
    Processus *courant = tete_liste;
    while (courant != NULL) {
        
        if (strcmp(courant->pid, pid) == 0) {
            return courant; 
        }
        courant = courant->suivant;
    }
    return NULL; 
}

unsigned long proc_get_time_cpu_system() {

    FILE *f_cpu_sys = fopen("/proc/stat", "r");
    if (!f_cpu_sys) return 0;

    char buffer[1024];
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long total_time = 0;

    if (fgets(buffer, sizeof(buffer), f_cpu_sys)) {

        if (strncmp(buffer, "cpu ", 4) == 0) {
            
            int n_read = sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", 
                                &user, &nice, &system, &idle, &iowait, &irq, &softirq, 
                                &steal, &guest, &guest_nice);
                                
            // verification qu'on a bien lu au moins 8 ou 10 valeurs (selon la version du noyau)
            if (n_read >= 8) {
                total_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
            }
        }
    }
    
    fclose(f_cpu_sys);
    return total_time;
}



void calculer_cpu(Processus *liste_actuelle_t2, Processus *liste_ancienne_t1, float temps_ecoule) {

    Processus *proc_t2 = liste_actuelle_t2;
    
    while (proc_t2 != NULL) {
        
        Processus *proc_t1 = trouver_processus_par_pid(liste_ancienne_t1, proc_t2->pid);
   

        if (proc_t1 !=NULL && temps_ecoule > 0) {
            
            unsigned long temps_total_t2 = proc_t2->utime + proc_t2->stime;
            unsigned long temps_total_t1 = proc_t1->utime + proc_t1->stime;
            
            unsigned long delta_temps_proc = temps_total_t2 - temps_total_t1;
            
            float cpu_utilisation = ((float)delta_temps_proc * 100.0f) / temps_ecoule;
            
            proc_t2->cpu_pc = cpu_utilisation; 

        } else {
            proc_t2->cpu_pc = 0.0f; 
        }

        proc_t2 = proc_t2->suivant;
    }
}


Processus* ajouter_processus(Processus *tete_liste, const char *pid_proc, const char *nom_proc, char etat, const char *user_proc, long ram_rss, unsigned long utime, unsigned long stime, float cpu_pc) {
    
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





Processus* liberer_liste(Processus *tete_liste) {
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
    
    Processus *liste_actuelle_t2 = NULL; // Liste des processus lue MAINTENANT (T2)
    Processus *liste_ancienne_t1 = NULL; // Liste des processus lue AVANT (T1)
    
    unsigned long total_sys_t1 = 0; // temps CPU total T1
    
    // BOUCLE INFINIE 
    while (1) {
                
        system("clear"); // Nettoyer l'ecran pour le rafraîchissement

        // Ouvrir le répertoire /proc a chaque tour
        DIR *rep_proc = opendir("/proc");
        struct dirent *ent;
        
        if (rep_proc == NULL) {
            perror("Erreur d'ouverture de /proc");
            sleep(1); // Attendre et réessayer
            continue; 
        }

        unsigned long total_sys_t2 = proc_get_time_cpu_system(); 
        
        while ((ent = readdir(rep_proc)) != NULL) {

            if (est_un_pid(ent->d_name)) {
                
                char nom_proc[260];
                char user_proc[32]; 
                long ram_proc = 0;
                char etat = '?';
                unsigned long utime_proc = 0; 
                unsigned long stime_proc = 0; 

                lire_nom_processus(ent->d_name, nom_proc, sizeof(nom_proc));
                etat = lire_etat_processus(ent->d_name);
                lire_utilisateur(ent->d_name, user_proc, sizeof(user_proc));
                ram_proc = lire_memoire_rss(ent->d_name);
                proc_get_cpu_time(ent->d_name, &utime_proc, &stime_proc); 

                liste_actuelle_t2 = ajouter_processus(
                    liste_actuelle_t2, 
                    ent->d_name, 
                    nom_proc, 
                    etat, 
                    user_proc, 
                    ram_proc, 
                    utime_proc, 
                    stime_proc, 
                    0.0f 
                ); 
            }
        }
        closedir(rep_proc); 

        
        float temps_ecoule = (float)(total_sys_t2 - total_sys_t1);

        if (liste_ancienne_t1 != NULL && total_sys_t1 > 0 && temps_ecoule > 0) {
            calculer_cpu(liste_actuelle_t2, liste_ancienne_t1, temps_ecoule); 
        }

        // AFFICHAGE 

        printf("MONITEUR PROCESSUS (Rafraîchissement: 1s)\n");

        printf("%-8s | %-20s | %-6s | %-15s | %-10s | %-6s\n", "PID", "NOM", "ETAT", "USER", "RAM (Ko)", "%CPU");

        Processus *courant = liste_actuelle_t2;

        while (courant != NULL) {
            printf("%-8s | %-20s | %-6c | %-15s | %-10ld | %-6.1f\n", 
                courant->pid, 
                courant->nom, 
                courant->etat,
                courant->user, 
                courant->ram_rss / 1024, 
                courant->cpu_pc
            );
            courant = courant->suivant;
        }
        
        if (liste_ancienne_t1 != NULL) {
            liste_ancienne_t1 = liberer_liste(liste_ancienne_t1);
        }
        
        liste_ancienne_t1 = liste_actuelle_t2;
        liste_actuelle_t2 = NULL; 
        
        total_sys_t1 = total_sys_t2;

        sleep(1); 
    }
    
    return 0;
}
/* #include <stdio.h>
#include <dirent.h> //dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <pwd.h>

#include "process.h"
#include <unistd.h>

bool str_is_numeric(const char *str) {
   	
	if(!str || *str == '\0') return false;

	if(*str == '-') ++str;

	while (*str) {
		if(!isdigit((unsigned char)(*str))) {
			return false;
		}
		++str;
	}
	return true;
}

bool proc_is_valid_pid(const char *pid) {
	return str_is_numeric(pid) && (*pid != '-' || *pid != '0');
}

FILE *proc_file_open(const pid_t pid, const char *file) {
	if(!file) return nullptr;

	char path[PROC_PATH_SIZE];

        snprintf(path, sizeof(path), "/proc/%d/%s", pid, file);

        return fopen(path, "r");
}

proc_err_t proc_get_name(const pid_t pid, char *name) {
	
	if(!name) return NULLPTR_PARAMETER_ERROR;
	
	FILE *f = proc_file_open(pid, "comm");

	if (!f) return OPEN_FILE_FAILED;

        if (!fgets(name, PROC_NAME_SIZE, f)) {
		fclose(f);
		return READ_FAILED;
	}
	
	const int end_of_str = strcspn(name, "\n");
	name[end_of_str] = '\0';

        fclose(f);

	return SUCCESS;
}

proc_err_t proc_get_state(const pid_t pid, proc_state_t *state) {
	
	if (!state) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(pid, "stat");

	if (!f) return OPEN_FILE_FAILED; 
        
	char buffer[512];
        
	if (!fgets(buffer, sizeof(buffer), f)) { 
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);

	char temp = '\0';

	/*
	 * %*d		--> ignore the first integer (pid)
	 * (		--> read the '(' character
	 * %*[^)]	--> ignore all character until ')' has been reached
	 * )		--> read the ')' character
	 * %c		--> get the character
	 */
	/*const int ret = sscanf(buffer, "%*d (%*[^)]) %c", &temp);

	if(ret != 1) return MALFORMED_STATUS_LINE; 
        
	switch(temp)
	{
		case 'R': *state = RUNNING;	break;
		case 'S': *state = SLEEPING;	break;
		case 'D': *state = DISK_SLEEP;	break;
		case 'T': *state = STOPPED;	break;
		case 't': *state = TRACED;	break;
		case 'Z': *state = ZOMBIE;	break;
		case 'X': *state = DEAD;	break;
		default:  *state = UNKNOW;
	}

    	return SUCCESS;
}

proc_err_t proc_get_user(const pid_t pid, char *username) {
    
	if(!username) return NULLPTR_PARAMETER_ERROR;
	 
	FILE *f = proc_file_open(pid, "status");

    	if (!f) return OPEN_FILE_FAILED;

	char line[512]; 
	uid_t uid = -1;
        
	while (fgets(line, sizeof(line), f)) {
            
		if (strncmp(line, "Uid:", 4) != 0) continue;

		const int ret = sscanf(line, "%*s %u", &uid);
               	if(ret != 1) return MALFORMED_STATUS_LINE;

                break;	
        }

        fclose(f);  
    	
	if(uid == (uid_t)-1) return UID_NOT_FOUND;

	struct passwd *user_info = getpwuid(uid);  

	if (!user_info) return USER_NOT_FOUND;

	strncpy(username,user_info->pw_name,PROC_USERNAME_SIZE);
	username[PROC_USERNAME_SIZE - 1] = '\0';

	return SUCCESS;
}

    
proc_err_t proc_get_rss(const pid_t pid, long* rss) {
	
	if(!rss) return NULLPTR_PARAMETER_ERROR;

	char line[200];

	FILE *f = proc_file_open(pid, "statm");

	if (!f) return OPEN_FILE_FAILED;

	if(fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);

	long resident = 0;
	unsigned long _;
	if(sscanf(line,"%lu %ld", &_, &resident) != 2) return MALFORMED_STATUS_LINE;
	
	const long page_size = sysconf(_SC_PAGESIZE);
	*rss = resident * page_size; 
	
	
	return SUCCESS;
}
    


void lire_temps_cpu_proc(const char *pid, unsigned long *utime, unsigned long *stime) {


}


Processus* ajouter_processus(Processus *tete_liste, const char *pid_proc, const char *nom_proc, char etat, const char *user_proc, long ram_rss, unsigned long utime, unsigned long stime, float cpu_percent)) {
    Processus *nouveau_proc = malloc(sizeof(Processus));

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

*/
