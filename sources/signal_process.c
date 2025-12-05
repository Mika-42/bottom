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
int send_signal(processus_t *p, int sig){
	int pidfd = syscall(SYS_pidfd_open, p->pid, 0);

	if (pidfd < 0) return EXIT_FAILURE;
	
	int ret = syscall(SYS_pidfd_send_signal, pidfd, sig, NULL, 0);
	
	close(pidfd);

	return (ret < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
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

	//printf("path exe : %s", exe_path);
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
	//for (int i = 0; i < argc ; ++i){
	//printf("arg %d : %s", i, argv[i]);
	//}
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
	
	if (new_pid == 0){
	execv(exe_path, argv);
	exit(EXIT_FAILURE);
	}
	
	p->pid = new_pid;
	return EXIT_SUCCESS;
}
