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

#define MAX_SIZE_PATH 1024
#define MAX_ARG 256

int send_signal(processus_t *p, int sig){
	int pidfd = syscall(SYS_pidfd_open, p->pid, 0);

	if (pidfd < 0){
		return EXIT_FAILURE;
	}
	if (syscall(SYS_pidfd_send_signal, pidfd, sig, NULL, 0) < 0){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int kill_process(processus_t *p){
	return send_signal(p, SIGKILL);
}

int stop_process(processus_t *p){
	return send_signal(p, SIGSTOP);
}

int term_process(processus_t *p){
	return send_signal(p, SIGTERM);
}

int get_exe(processus_t *p, char *exe_path, int size){
	char proc_link[MAX_SIZE_PATH];
	snprintf(proc_link, sizeof(proc_link), "/proc/%d/exe", p->pid);
	ssize_t len = readlink(proc_link, exe_path, size -1);
	if (len < 0) return EXIT_FAILURE;
	exe_path[len] = '\0';
	return EXIT_SUCCESS;
}
int get_arg(processus_t *p, char *argv[], int max_arg){
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
	while (b < buffer + n && argc < max_arg - 1){
		argv[argc++] = b;
		b += strlen(b) +1;
	}
	argv[argc] = NULL;
	return EXIT_SUCCESS;
}

int restart_process(processus_t *p){
	char exe_path[MAX_SIZE_PATH];
	char *argv[MAX_ARG];

	if (get_exe(p, exe_path, sizeof(exe_path)) != EXIT_SUCCESS) return EXIT_FAILURE;
	if (get_arg(p, argv, MAX_ARG) != EXIT_SUCCESS) return EXIT_FAILURE;


	if (term_process(p) != EXIT_SUCCESS) return EXIT_FAILURE;
	if (waitpid(p->pid, NULL, 0) < 0) return EXIT_FAILURE;

	pid_t new_pid = fork();
	if (new_pid < 0) return EXIT_FAILURE;

	execv(exe_path, argv);

	return EXIT_SUCCESS;
}
