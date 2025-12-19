#ifndef PROCESSUS_SIGNAL_H
#define PROCESSUS_SIGNAL_H

#include "processus.h"
#include "error.h"

[[nodiscard]] bool pid_exists(pid_t pid);
[[nodiscard]] bool pid_does_not_exists(pid_t pid);
[[maybe_unused]] error_code_t proc_kill(processus_t *p);
[[maybe_unused]] error_code_t proc_stop(processus_t *p);
[[maybe_unused]] error_code_t proc_cont(processus_t *p);
[[maybe_unused]] error_code_t proc_term(processus_t *p);
[[maybe_unused]] error_code_t proc_restart(processus_t *p);

//todo optimize 
[[nodiscard]] error_code_t kill_children(processus_t* p);

#endif //PROCESSUS_SIGNAL_H
