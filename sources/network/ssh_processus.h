#ifndef SSH_PROCESSUS_H
#define SSH_PROCESSUS_H

#include <time.h>
#include <sys/types.h>
#include "error.h"
#include "ssh_connexion.h"

[[nodiscard]] error_code_t ssh_get_user(processus_t *proc, ssh_session session);
[[nodiscard]] error_code_t ssh_get_stat(processus_t *proc, ssh_session session);
[[nodiscard]] error_code_t ssh_get_global_stat(long *cpu_total, time_t *boot_time, ssh_session session);
[[nodiscard]] error_code_t ssh_get_all_infos(const pid_t pid, processus_t *proc, ssh_session session);

#endif //SSH_PROCESSUS_H
