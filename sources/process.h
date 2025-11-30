#ifndef PROCESS_H
#define PROCESS_H

#include <limits.h>
#include <sys/types.h>

#define PROC_NAME_SIZE 16
#define PROC_USERNAME_SIZE 32

typedef enum proc_err_t : int {
	success,
	generic_error,
	nullptr_parameter_error,
	open_file_failed,
	read_failed,
	malformed_status_line,
	uid_not_found,
	user_not_found,
} proc_err_t;

typedef enum proc_state_t : char {
	running		= 'R',
	sleeping	= 'S',
	disk_sleep	= 'D',
	stopped		= 'T',
	traced		= 't',
	zombie		= 'Z',
	dead		= 'X',
	unknow		= '\0',
} proc_state_t;

// Structure pour stocker les informations d'un seul processus
typedef struct processus_t {
	pid_t		pid;
	char		name[PROC_NAME_SIZE];
	proc_state_t	state;
	long		ram_rss;

    char user[PROC_USERNAME_SIZE];      // Nom de l'utilisateur
    unsigned long utime;    // Temps CPU utilisateur (14e valeur)
    unsigned long stime;    // Temps CPU système (15e valeur)
    float cpu_percent;
    
    struct processus_t *suivant; // Pointeur vers le processus suivant (le chaînon)
} processus_t;

bool		str_is_numeric(const char *str);
bool		proc_is_valid_pid(const char *pid);
proc_err_t	proc_get_name(const pid_t pid, char *name);
proc_err_t	proc_get_state(const pid_t pid, proc_state_t *state);
proc_err_t	proc_get_user(const pid_t pid, char *username);
proc_err_t	proc_get_rss(const pid_t pid, long* rss);

#endif //PROCESS_H 
