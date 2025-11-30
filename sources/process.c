#include <stdio.h>
#include <dirent.h> //dirent.h c'est une structure permettant de parcourir le repertoire /proc, de l'ouvrir et le lire 
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>


#include "process.h"

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

bool proc_is_valid_pid(const char* pid)
{
	return str_is_numeric(pid) && (*pid != '-' || *pid != '0');
}

FILE* proc_file_open(const pid_t pid, const char* file)
{
	if(!file) return nullptr;

	char path[PATH_MAX];

        snprintf(path, sizeof(path), "/proc/%d/%s", pid, file);

        return fopen(path, "r");
}

proc_err_t proc_get_name(const pid_t pid, char *name) {
	
	if(!name) return proc_err_t::nullptr_parameter_error;
	
	FILE *f = proc_file_open(pid, "comm");

	if (!f) return proc_err_t::open_file_failed;

        if (!fgets(name, PROC_NAME_SIZE, f)) {
		fclose(f);
		return proc_err_t::read_failed;
	}
	
	const int end_of_str = strcspn(name, "\n");
	name[end_of_str] = '\0';

        fclose(f);

	return proc_err_t::success;
}

proc_err_t proc_get_state(const char *pid, proc_state_t *state) {
	
	if (!state) return proc_err_t::nullptr_parameter_error;

	FILE *f = proc_file_open(pid, "stat");

	if (!f) return proc_err_t::open_file_failed; 
        
	char buffer[512];
        
	if (!fgets(buffer, sizeof(buffer), f)) { 
		fclose(f);
		return proc_err_t::read_failed;
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
	const int ret = sscanf(buffer, "%*d (%*[^)]) %c", &temp);

	if(ret != 1) return proc_err_t::malformed_status_line; 
        
	switch(temp)
	{
		case 'R': *state = running;	break;
		case 'S': *state = sleeping;	break;
		case 'D': *state = disk_sleep;	break;
		case 'T': *state = stopped;	break;
		case 't': *state = traced;	break;
		case 'Z': *state = zombie;	break;
		case 'X': *state = dead;	break;
		default:  *state = unknow;
	}

    	return proc_err_t::success;
}

proc_err_t proc_get_user(const pid_t pid, char *username){
    
	if(!username) return proc_err_t::nullptr_parameter_error;
	 
	FILE *f = proc_file_open(pid, "status");

    	if (!f) return proc_err_t::open_file_failed;

	char uid_line[512]; 
	uid_t uid = -1;
        
	while (fgets(uid_line, sizeof(uid_line), f)) {
            
		if (strncmp(uid_line, "Uid:", 4) != 0) continue;

		const int ret = sscanf(uid_line, "%*s %u", &uid);
               	if(ret != 1) return proc_err_t::malformed_status_line;

                break;	
        }

        fclose(f);  
    	
	if(uid == -1) return proc_err_t::uid_not_found;

	struct passwd *user_info = getpwuid(uid);  

	if (!user_info) return proc_err_t::user_not_found;

	strncpy(username,user_info->pw_name,PROC_USERNAME_SIZE);
	username[PROC_USERNAME_SIZE - 1] = '\0';

	return proc_err_t::success;
}

    
proc_err_t proc_get_rss(const pid_t pid, long* rss){
	
	if(!rss) return proc_err_t::nullptr_parameter_error;

	char line[200];

	FILE *f = proc_file_open(pid, "statm");

	if (!f) return proc_err_t::open_file_failed;

	if(fgets(line, sizeof(line), f)) {
		fclose(f);
		return proc_err_t::read_failed;
	}

	long resident = 0;
	sscanf(ligne,"%*lu %ld", &resident);
	const long page_size = sysconf(_SC_PAGESIZE);
	*rss = resident * page_size; 
	
	fclose(f);
	
	return proc_err_t::success;
}
    


void lire_temps_cpu_proc(const char *pid, unsigned long *utime, unsigned long *stime){


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
