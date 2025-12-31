#ifndef SSH_PROCESSUS_H
#define SSH_PROCESSUS_H

#include <time.h>
#include <sys/types.h>
#include "error.h"
#include "ssh_connexion.h"
#include "processus_array.h"

[[nodiscard]] error_code_t ssh_array_update(processus_array_t *array, ssh_session session);

#endif //SSH_PROCESSUS_H
