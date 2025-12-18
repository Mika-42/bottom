#ifndef PROCESSUS_SIGNAL_H
#define PROCESSUS_SIGNAL_H

#include "processus.h"
#include "error.h"

[[nodiscard]] bool pid_exists(pid_t pid);
[[nodiscard]] bool pid_does_not_exists(pid_t pid);
[[nodiscard]] error_code_t proc_kill(processus_t *p);
[[nodiscard]] error_code_t proc_stop(processus_t *p);
[[nodiscard]] error_code_t proc_cont(processus_t *p);
[[nodiscard]] error_code_t proc_term(processus_t *p);
[[nodiscard]] error_code_t proc_restart(processus_t *p);
[[nodiscard]] int get_exe(processus_t* p, char* exe_path, int size);
[[nodiscard]] int get_arg(processus_t* p, char* argv[], int max_arg);
[[nodiscard]] int get_env(processus_t* p, char* envp[], int max_env);
[[nodiscard]] int kill_children(processus_t* p);

#endif //PROCESSUS_SIGNAL_H
