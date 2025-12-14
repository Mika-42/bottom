#ifndef PROCESSUS_SIGNAL_H
#define PROCESSUS_SIGNAL_H

#include "processus.h"
#include "error.h"

bool DO_NOT_IGNORE pid_exists(pid_t pid);
bool DO_NOT_IGNORE pid_does_not_exists(pid_t pid);
 
error_code_t DO_NOT_IGNORE proc_kill(processus_t *p);
error_code_t DO_NOT_IGNORE proc_stop(processus_t *p);
error_code_t DO_NOT_IGNORE proc_cont(processus_t *p);
error_code_t DO_NOT_IGNORE proc_term(processus_t *p);
error_code_t DO_NOT_IGNORE proc_restart(processus_t *p);

#endif //PROCESSUS_SIGNAL_H
