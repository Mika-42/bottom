#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

constexpr size_t PROC_NAME_SIZE = 16;
constexpr size_t PROC_USERNAME_SIZE = 32;
constexpr size_t PROC_PATH_SIZE = 4096;

typedef enum proc_err_t : int {
	SUCCESS,
	GENERIC_ERROR,
	NULLPTR_PARAMETER_ERROR,
	OPEN_FILE_FAILED,
	READ_FAILED,
	MALFORMED_STATUS_LINE,
	UID_NOT_FOUND,
	USER_NOT_FOUND,
	MEMORY_ALLOCATION_FAILED,
} proc_err_t;

typedef enum proc_state_t : char {
	RUNNING		= 'R',
	SLEEPING	= 'S',
	DISK_SLEEP	= 'D',
	STOPPED		= 'T',
	TRACED		= 't',
	ZOMBIE		= 'Z',
	DEAD		= 'X',
	UNKNOW		= '\0',
} proc_state_t;

// Structure pour stocker les informations d'un seul processus
typedef struct processus_t {  
	pid_t			pid;
	char			name[PROC_NAME_SIZE];
	proc_state_t		state;
	long			ram_rss;
	char			user[PROC_USERNAME_SIZE];
    	unsigned long		utime;    // Temps CPU utilisateur
    	unsigned long		stime;    // Temps CPU système
} processus_t;

typedef struct element_t {
	processus_t data;
	struct element_t *next;
} element_t;

typedef element_t* list_t;

bool		str_is_numeric(const char *str);
bool		proc_is_valid_pid(const char *pid);
proc_err_t	proc_get_name(const pid_t pid, char *name);
proc_err_t	proc_get_state(const pid_t pid, proc_state_t *state);
proc_err_t	proc_get_user(const pid_t pid, char *username);
proc_err_t	proc_get_rss(const pid_t pid, long* rss);
proc_err_t	proc_get_cpu_time(const pid_t pid, unsigned long *utime, unsigned long *stime);
#endif //PROCESS_H 
