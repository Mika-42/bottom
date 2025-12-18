#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <time.h>
#include <sys/types.h>


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

[[nodiscard]] bool str_is_numeric(const char *str);
[[nodiscard]] bool proc_is_valid_pid(const char *pid);
[[nodiscard]] error_code_t proc_get_user(processus_t *proc);
[[nodiscard]] error_code_t proc_get_stat(processus_t *proc);
[[nodiscard]] error_code_t proc_get_global_stat(long *cpu_total, time_t *boot_time);
[[nodiscard]] error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc);

int read_processus(char *buffer, processus_t *p);
int parse_stat(char *buffer, processus_t *p);
int parse_status(char *buffer, processus_t *p);
char* ssh_exec_and_read(LIBSSH2_CHANNEL *channel, const char *command);
#endif //PROCESSUS_H
