#define _GNU_SOURCE

#include "processus_signal.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

bool pid_exists(pid_t pid) {
    int pidfd = syscall(SYS_pidfd_open, pid, 0);

    if (pidfd < 0)
        return errno != ESRCH;

    close(pidfd);

    return true;
}

bool pid_does_not_exists(pid_t pid) {
    return !pid_exists(pid);
}

error_code_t send_signal(processus_t* p, int sig) {

    if (!p)
        return NULLPTR_PARAMETER_ERROR;

    int pidfd = syscall(SYS_pidfd_open, p->pid, 0);
    if (pidfd < 0)
        return SEND_SIGNAL_PIDFD_OPEN_FAILED;

    int ret = syscall(SYS_pidfd_send_signal, pidfd, sig, NULL, 0);
    close(pidfd);

    if (ret < 0)
        return SEND_SIGNAL_SEND_FAILED;

    if (sig != SIGKILL && sig != SIGTERM)
        return SUCCESS;

    if (p->ppid != getpid())
        return SUCCESS;

    if (waitpid(p->pid, nullptr, 0) < 0)
        return SEND_SIGNAL_WAIT_FAILED;

    return SUCCESS;
}

error_code_t proc_kill(processus_t* p) {
    return send_signal(p, SIGKILL);
}

error_code_t proc_stop(processus_t* p) {
    return send_signal(p, SIGSTOP);
}

error_code_t proc_cont(processus_t* p) {
    return send_signal(p, SIGCONT);
}

error_code_t proc_term(processus_t* p) {
    return send_signal(p, SIGTERM);
}

int kill_children(processus_t* p) {
    char path[PROC_PATH_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", p->pid, p->pid);
    FILE* f = fopen(path, "r");
    if (!f)
        return EXIT_FAILURE;

    pid_t child;
    while (fscanf(f, "%d", &child) == 1) {
        int pidfd = syscall(SYS_pidfd_open, child, 0);
        if (pidfd < 0)
            continue;
        processus_t proc = {.pid = child};
        if(kill_children(&proc) == EXIT_SUCCESS) {
        syscall(SYS_pidfd_send_signal, pidfd, SIGKILL, NULL, 0);
		}
        close(pidfd);
    }
    fclose(f);

    return EXIT_SUCCESS;
}

error_code_t proc_restart(processus_t* p) {
    if (pid_does_not_exists(p->pid)) {
        return EXIT_FAILURE;
    }
    
	if (kill_children(p) != EXIT_SUCCESS || proc_term(p) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (p->ppid == getpid()) {
        waitpid(p->pid, NULL, 0);
    } else {
        int timeout_ms = 5000;
        int waited = 0;
        while (pid_exists(p->pid) && waited < timeout_ms) {
            usleep(10000);
            waited += 10;
        }

        if (pid_exists(p->pid)) {
            return EXIT_FAILURE;
        }
    }

    pid_t new_pid = fork();
    if (new_pid < 0) {
        return EXIT_FAILURE;
    }
    if (new_pid == 0) {
	
		size_t i = 0;
		
		char *argv[PROC_CMD_COUNT + 1];
		for (i = 0; i < PROC_CMD_COUNT && p->cmdline[i][0] != '\0'; ++i)
			argv[i] = p->cmdline[i];
		argv[i] = NULL;

		char *env[PROC_CMD_COUNT + 1];
		for (i = 0; i < PROC_CMD_COUNT && p->env[i][0] != '\0'; ++i)
			env[i] = p->env[i];
		env[i] = NULL;

		execve(p->executable, argv, env);
        exit(EXIT_FAILURE);
    }

    p->pid = new_pid;
    usleep(100000);
    if (pid_does_not_exists(p->pid)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
