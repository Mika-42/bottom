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

constexpr size_t MAX_SIZE_PATH = 1024;
constexpr size_t MAX_ARG = 256;

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

static void free_str_array(char* arr[]) {
    if (!arr)
        return;
    for (int i = 0; arr[i]; ++i)
        free(arr[i]);
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

// TODO refacto the code below
int get_exe(processus_t* p, char* exe_path, int size) {
    char proc_link[MAX_SIZE_PATH];
    snprintf(proc_link, sizeof(proc_link), "/proc/%d/exe", p->pid);
    ssize_t len = readlink(proc_link, exe_path, size - 1);
    if (len < 0)
        return EXIT_FAILURE;
    exe_path[len] = '\0';

    return EXIT_SUCCESS;
}
/******/
int get_arg(processus_t* p, char* argv[], int max_arg) {
    char path[MAX_SIZE_PATH];
    snprintf(path, sizeof(path), "/proc/%d/cmdline", p->pid);
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return EXIT_FAILURE;

    char buffer[4096];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    if (n <= 0)
        return EXIT_FAILURE;
    buffer[n] = '\0';

    int argc = 0;
    char* b = buffer;
    while (b < buffer + n && argc < max_arg - 1) {
        argv[argc] = strdup(b);
        if (argv[argc] == NULL) {
            for (int i = 0; i < argc; ++i)
                free(argv[i]);
            return EXIT_FAILURE;
        }
        ++argc;
        b += strlen(b) + 1;
    }
    argv[argc] = NULL;
    return EXIT_SUCCESS;
}

int get_env(processus_t* p, char* envp[], int max_env) {
    char path[MAX_SIZE_PATH];
    snprintf(path, sizeof(path), "/proc/%d/environ", p->pid);
    int fd = open(path, O_RDONLY);
    
    if (fd < 0)
        return EXIT_FAILURE;

    char buffer[8192];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    if (n <= 0)
        return EXIT_FAILURE;
    buffer[n] = '\0';

    int count = 0;
    char* b = buffer;
    while (b < buffer + n && count < max_env - 1) {
        envp[count] = strdup(b);
	/*ICI*/
	++count;
	b += strlen(b) + 1;
    }
    envp[count] = NULL;
    return EXIT_SUCCESS;
}
/****/

int kill_children(processus_t* p) {
    char path[MAX_SIZE_PATH];
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
        kill_children(&proc);
        syscall(SYS_pidfd_send_signal, pidfd, SIGKILL, NULL, 0);
        close(pidfd);
    }
    fclose(f);

    return EXIT_SUCCESS;
}

error_code_t proc_restart(processus_t* p) {
    if (pid_does_not_exists(p->pid)) {
        return EXIT_FAILURE;
    }

    char exe_path[MAX_SIZE_PATH];
    char* argv[MAX_ARG];
    char* envp[MAX_ARG];

    if (get_exe(p, exe_path, sizeof(exe_path)) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    if (get_arg(p, argv, MAX_ARG) != EXIT_SUCCESS) {
        free_str_array(argv);
        free_str_array(envp);
        return EXIT_FAILURE;
    }
    if (get_env(p, envp, MAX_ARG) != EXIT_SUCCESS) {
        free_str_array(argv);
        free_str_array(envp);
        return EXIT_FAILURE;
    }

    if (kill_children(p) != EXIT_SUCCESS || proc_term(p) != EXIT_SUCCESS) {
        free_str_array(argv);
        free_str_array(envp);
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
            free_str_array(argv);
            free_str_array(envp);
            return EXIT_FAILURE;
        }
    }

    pid_t new_pid = fork();
    if (new_pid < 0) {
        free_str_array(argv);
        free_str_array(envp);
        return EXIT_FAILURE;
    }
    if (new_pid == 0) {
        execve(exe_path, argv, envp);
        exit(EXIT_FAILURE);
    }

    p->pid = new_pid;
        free_str_array(argv);
        free_str_array(envp);
    usleep(100000);
    if (pid_does_not_exists(p->pid)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
