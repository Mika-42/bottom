#include "signal_process.h"

#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

int send_signal(processus_t *p, int sig){
	pid_t pid = p->pid;

	if (pid <= 0){
		return -1;
	}
	if (kill(pid, sig) == -1 ){
		return -1;
	}
	return 0;
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
