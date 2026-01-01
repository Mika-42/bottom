#ifndef SSH_PROCESSUS_H
#define SSH_PROCESSUS_H

#include <time.h>
#include <sys/types.h>
#include "error.h"
#include "ssh_connexion.h"
#include "processus_array.h"

/**
 * @brief Met à jour les information des processus en connexion SSH
 *
 * @param array Tableau de processus à mettre à jour
 * @param session Sessioin utilisé pour la communication
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t ssh_array_update(processus_array_t *array, ssh_session session);

#endif //SSH_PROCESSUS_H
