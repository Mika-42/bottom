#define _GNU_SOURCE

#include "signal_process.h"

#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

constexpr size_t MAX_SIZE_PATH = 1024;
constexpr size_t MAX_ARG = 256;

bool pid_exists(pid_t pid) {
	int pidfd = syscall(SYS_pidfd_open, pid, 0);

	if (pidfd < 0) return errno != ESRCH;
	
	close(pidfd); 
	
	return true;
}

bool pid_does_not_exists(pid_t pid) {
	return !pid_exists(pid);
}

int send_signal(processus_t *p, int sig) {
	int pidfd = syscall(SYS_pidfd_open, p->pid, 0);

	if (pidfd < 0) return EXIT_FAILURE;
	
	int ret = syscall(SYS_pidfd_send_signal, pidfd, sig, NULL, 0);
	
	close(pidfd);

	return (ret < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}

int kill_process(processus_t *p) {
	return send_signal(p, SIGKILL);
}

int stop_process(processus_t *p) {
	return send_signal(p, SIGSTOP);
}

int cont_process(processus_t *p) {
	return send_signal(p, SIGCONT);
}

int term_process(processus_t *p) {
	return send_signal(p, SIGTERM);
}

int get_exe(processus_t *p, char *exe_path, int size);
int get_arg(processus_t *p, char *argv[], int max_arg);
int get_env(processus_t *p, char *envp[], int max_env);

int get_exe(processus_t *p, char *exe_path, int size) {
	char proc_link[MAX_SIZE_PATH];
	snprintf(proc_link, sizeof(proc_link), "/proc/%d/exe", p->pid);
	ssize_t len = readlink(proc_link, exe_path, size -1);
	if (len < 0) return EXIT_FAILURE;
	exe_path[len] = '\0';

	return EXIT_SUCCESS;
}
int get_arg(processus_t *p, char *argv[], int max_arg) {
	char path[MAX_SIZE_PATH];

	snprintf(path, sizeof(path), "/proc/%d/cmdline", p->pid);
	int fd = open(path, O_RDONLY);
	if (fd < 0) return EXIT_FAILURE;

	char buffer[4096];
	ssize_t n = read(fd, buffer, sizeof(buffer)-1);
	close(fd);
	if (n <= 0) return EXIT_FAILURE;
	buffer[n] = '\0';

	int argc = 0;
	char *b = buffer;
	while (b < buffer + n && argc < max_arg - 1) {
		argv[argc] = strdup(b);
		if (argv[argc] == NULL) {
			for (int i = 0; i < argc; ++i) free(argv[i]);
			return EXIT_FAILURE;
		}
		argc++;
		b += strlen(b) +1;
	}
	argv[argc] = NULL;
	return EXIT_SUCCESS;
}

int get_env(processus_t *p, char *envp[], int max_env) {
    	char path[MAX_SIZE_PATH];
   	
	snprintf(path, sizeof(path), "/proc/%d/environ", p->pid);
    	int fd = open(path, O_RDONLY);
    	if (fd < 0) return EXIT_FAILURE;

    	char buffer[8192];
    	ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    	close(fd);
    	if (n <= 0) return EXIT_FAILURE;
	buffer[n] = '\0';

    	int count = 0;
    	char *b = buffer;
    	while (b < buffer + n && count < max_env - 1) {
        	envp[count++] = strdup(b);
        	b += strlen(b) + 1;
    	}
    	envp[count] = NULL;
    	return EXIT_SUCCESS;
}


int restart_process(processus_t *p) {

	if (pid_does_not_exists(p->pid)) {
		printf( "Erreur : Le processus avec PID %d n'existe pas.\n", p->pid);
		return EXIT_FAILURE;
	}
       	
	char exe_path[MAX_SIZE_PATH];
	char *argv[MAX_ARG];
	char *envp[MAX_ARG];

	if (get_exe(p, exe_path, sizeof(exe_path)) != EXIT_SUCCESS) {
		printf( "Erreur : Impossible de récupérer le chemin de l'exécutable pour PID %d.\n", p->pid);
		return EXIT_FAILURE;
	}
	if (get_arg(p, argv, MAX_ARG) != EXIT_SUCCESS) {
		printf( "Erreur : Impossible de récupérer les arguments pour PID %d.\n", p->pid);
		return EXIT_FAILURE;
	}
	if (get_env(p, envp, MAX_ARG) != EXIT_SUCCESS) {
		printf("Erreur : Impossible de récupérer l'environnement pour PID %d.\n", p->pid);
		return EXIT_FAILURE;
	}

	if (term_process(p) != EXIT_SUCCESS) {
		for (int i = 0; argv[i] != NULL; ++i) free(argv[i]);
		for (int i = 0; envp[i] != NULL; ++i) free(envp[i]);
		return EXIT_FAILURE;
	}
	
	int timeout_ms = 5000;
	int waited = 0;
	while (pid_exists(p->pid) && waited < timeout_ms) {
		usleep(10000);
		waited += 10;
	}
	if (pid_exists(p->pid)) {
		printf("Erreur : Le processus %d ne s'est pas terminé dans le délai.\n", p->pid);
		for (int i = 0; argv[i] != NULL; ++i) free(argv[i]);
		for (int i = 0; envp[i] != NULL; ++i) free(envp[i]);
		return EXIT_FAILURE;
	}

	pid_t new_pid = fork();
	if (new_pid < 0) {
		printf("Fork failed");
		for (int i = 0; argv[i] != NULL; ++i) free(argv[i]);
		for (int i = 0; envp[i] != NULL; ++i) free(envp[i]);
		return EXIT_FAILURE;
	}
	if (new_pid == 0) {		
		execve(exe_path, argv, envp);
		printf("Child: errno=%d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	p->pid = new_pid;
	for (int i = 0; argv[i] != NULL; ++i) free(argv[i]);
	for (int i = 0; envp[i] != NULL; ++i) free(envp[i]);
	usleep(100000);
	if (pid_does_not_exists(p->pid)) {
		printf("Erreur : Le nouveau processus %d s'est terminé immédiatement.\n", new_pid);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
