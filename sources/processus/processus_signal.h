#ifndef PROCESSUS_SIGNAL_H
#define PROCESSUS_SIGNAL_H

#include "processus.h"

bool pid_exists(pid_t pid);
bool pid_does_not_exists(pid_t pid);

int proc_kill(processus_t *p);
int proc_stop(processus_t *p);
int proc_cont(processus_t *p);
int proc_term(processus_t *p);
int proc_restart(processus_t *p);

#endif //PROCESSUS_SIGNAL_H
