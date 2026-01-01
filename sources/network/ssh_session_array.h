#ifndef SSH_SESSION_ARRAY_H
#define SSH_SESSION_ARRAY_H

#include "ssh_connexion.h"
/**
 * @brief Tableau des sessions SSH utilisé pour ce connecter au différentes machine
 */
typedef struct ssh_session_array_t {
    ssh_session *data;		/**< Tableau de session */
    size_t size;		/**< Nombre de sessions dans le tableau */
    size_t capacity;		/**< Capacité maximum d'éléments du tableau (taille allouée) */
} ssh_session_array_t;

/**
 * @brief Inisialise un tableau de session
 *
 * @param array Tableau à initialiser
 */
void ssh_array_init(ssh_session_array_t *array);

/**
 * @brief Ajoute une session ssh au tableau de session
 *
 * @param array Tableau de session
 * @param session Session à ajouter au tableau
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] ssh_session *ssh_array_add(ssh_session_array_t *array, ssh_session session);

/**
 * @brief Libère la mémoire allouée à un tableau de session
 *
 * @param array Tableau à libérer
 */
void ssh_array_free(ssh_session_array_t *array);

#endif /* SSH_SESSION_ARRAY_H */
