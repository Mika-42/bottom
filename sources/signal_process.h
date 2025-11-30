#ifndef SIGNAL_PROCESS_H
#define SIGNAL_PROCESS_H

#include "process.h"

void kill_process(processus_t p);

void stop_process(processus_t p);

void term_process(processus_t p);

void reboot_process(processus_t p);

#endif //SIGNAL_PROCESS_H
