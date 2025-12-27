#ifndef SSH_CONNEXION
#define SSH_CONNEXION

#include <libssh/libssh.h>
#include "error.h"
#include "processus.h"

void ssh_end_session(ssh_session session);

ssh_session ssh_connexion_init(const char *host, int port, const char *user, const char *password);

error_code_t ssh_dry_run(ssh_session session);
error_code_t ssh_kill_processus(ssh_session session, int pid);
error_code_t ssh_term_processus(ssh_session session, int pid);
error_code_t ssh_stop_processus(ssh_session session, int pid);
error_code_t ssh_cont_processus(ssh_session session, int pid);
error_code_t ssh_restart_processus(ssh_session session, processus_t *p);

error_code_t ssh_get_file(ssh_session session, char *buffer, size_t buffer_size, const char *file);
error_code_t ssh_get_exe(ssh_session session, char *buffer, size_t buffer_size, processus_t *p);
error_code_t ssh_get_pid_list(ssh_session session, pid_t *pid_list, size_t size_list, size_t *count);
#endif //SSH_CONNEXION
