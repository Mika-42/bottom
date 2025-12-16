#ifndef SSH_CONNEXION
#define SSH_CONNEXION

#include <libssh/libssh.h>

void ssh_end_session(ssh_session session);


ssh_session ssh_connexion_init(char *host, int port, char *user, char *password);

int ssh_dry_run(ssh_session session); //EXIT_SUCCESS si réussi sinon EXIT_FAILURE

#endif //SSH_CONNEXION
