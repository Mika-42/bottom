#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <time.h>
#include <sys/types.h>
#include "error.h"

constexpr size_t PROC_NAME_SIZE = 256;
constexpr size_t PROC_USERNAME_SIZE = 32;
constexpr size_t PROC_PATH_SIZE = 4096;

/**
 * @brief Represente un processus avec toutes les informations dont on a besoin.
 */
typedef struct processus_t {  
	pid_t		pid;				/**< ID du processus */
	pid_t		ppid;				/**< ID du processus parent */
	char		name[PROC_NAME_SIZE];		/**< Nom du processus */
	char		state;				/**< Etat du processus */
	unsigned long long ram;				/**< Mémoire utilisé en octets par le processus */
	char		user[PROC_USERNAME_SIZE];	/**< Nom de l'utilisateur */
	unsigned long	utime;				/**< Temps CPU utilisateur */
	unsigned long	stime;				/**< Temps CPU système */
	unsigned long	start_time;			/**< Durée de lancement */
	double		cpu_usage;			/**< Pourcentage du CPU utilisé */
} processus_t;

bool		DO_NOT_IGNORE str_is_numeric(const char *str);
bool		DO_NOT_IGNORE proc_is_valid_pid(const char *pid);
error_code_t	DO_NOT_IGNORE proc_get_user(processus_t *proc);
error_code_t	DO_NOT_IGNORE proc_get_stat(processus_t *proc);
error_code_t	DO_NOT_IGNORE proc_get_global_stat(long *cpu_total, time_t *boot_time);
error_code_t	DO_NOT_IGNORE proc_get_all_infos(const pid_t pid, processus_t *proc);

#endif //PROCESSUS_H
