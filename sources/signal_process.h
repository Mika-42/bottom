#ifndef SIGNAL_PROCESS_H
#define SIGNAL_PROCESS_H

#include "process.h"

int kill_process(processus_t *p);

int stop_process(processus_t *p);

int term_process(processus_t *p);

int reboot_process(processus_t *p);

#endif //SIGNAL_PROCESS_H
