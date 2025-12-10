#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <sys/types.h>
#include "error.h"

constexpr size_t PROC_NAME_SIZE = 256;
constexpr size_t PROC_USERNAME_SIZE = 32;
constexpr size_t PROC_PATH_SIZE = 4096;

typedef struct processus_t {  
	pid_t		pid;
	char		name[PROC_NAME_SIZE];
	char		state;
	unsigned long long ram;
	char		user[PROC_USERNAME_SIZE];
	unsigned long	utime;    // Temps CPU utilisateur
	unsigned long	stime;    // Temps CPU système
	unsigned long	start_time;
	double		cpu_usage;
} processus_t;

bool		str_is_numeric(const char *str);
bool		proc_is_valid_pid(const char *pid);
error_code_t	proc_get_user(processus_t *proc);
error_code_t	proc_get_stat(processus_t *proc);
error_code_t	proc_get_cpu_total(long *cpu_total);
error_code_t	proc_get_all_infos(const pid_t pid, processus_t *proc);

#endif //PROCESSUS_H
