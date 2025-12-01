#define _GNU_SOURCE
#include "signal_process.h"

#include <signal.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int send_signal(processus_t *p, int sig){
	pid_t pid = p->pid;
	int pidfd = syscall(SYS_pidfd_open, pid, 0);

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
