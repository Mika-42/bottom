#ifndef SIGNAL_PROCESS_H
#define SIGNAL_PROCESS_H

#include "processus.h"

bool pid_exists(pid_t pid);

int kill_process(processus_t *p);

int stop_process(processus_t *p);

int term_process(processus_t *p);

int restart_process(processus_t *p);

#endif //SIGNAL_PROCESS_H
