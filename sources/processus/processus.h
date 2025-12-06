#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <sys/types.h>
#include "error.h"

constexpr size_t PROC_NAME_SIZE = 32;
constexpr size_t PROC_USERNAME_SIZE = 32;
constexpr size_t PROC_PATH_SIZE = 4096;

// https://stackoverflow.com/questions/29991182/programmatically-read-all-the-processes-status-from-proc
typedef enum proc_state_t : char {
	RUNNING		= 'R',
	SLEEPING	= 'S',
	DISK_SLEEP	= 'D',	
	ZOMBIE		= 'Z',
	STOPPED		= 'T',
	TRACED		= 't',
	WAKING		= 'W',
	WAKEKILL	= 'K',
	PARKED		= 'P',
	DEAD		= 'X',
	IDLE		= 'I',
	UNKNOW		= '\0',
} proc_state_t;

typedef struct processus_t {  
	pid_t			pid;
	char			name[PROC_NAME_SIZE];
	proc_state_t		state;
	long			ram_rss;
	char			user[PROC_USERNAME_SIZE];
    	unsigned long		utime;    // Temps CPU utilisateur
    	unsigned long		stime;    // Temps CPU système
} processus_t;

bool		str_is_numeric(const char *str);
bool		proc_is_valid_pid(const char *pid);
error_code_t	proc_get_name(processus_t *proc);
error_code_t	proc_get_state(processus_t *proc);
error_code_t	proc_get_user(processus_t *proc);
error_code_t	proc_get_rss(processus_t *proc);
error_code_t	proc_get_cpu_time(processus_t *proc);
error_code_t	proc_get_all_infos(const pid_t pid, processus_t *proc);
       
#endif //PROCESSUS_H     
