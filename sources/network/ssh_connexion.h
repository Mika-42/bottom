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

int ssh_get_file(ssh_session session, char **buffer, const char *file);
#endif //SSH_CONNEXION
