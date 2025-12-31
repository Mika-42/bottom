#ifndef SSH_CONNEXION
#define SSH_CONNEXION

#include <libssh/libssh.h>
#include "error.h"
#include "processus.h"

/**
 * @brief Permet de se déconnecter, fermer et liberer une session ssh
 *
 * @param session La session à finir
 */
void ssh_end_session(ssh_session session);

/**
 * @brief Initialise une connexion ssh avec une machine distante grâce à un mot de passe
 *
 * @param host Adresse IP ou adresse DNS de la machine distante, peu aussi être localhost
 * @param port Port de connexion utilisé pour la connexion
 * @param user Nom d'utilisateur pour ce connecter à la machine distante
 * @param password Mot de passe pour ce connecter à la machine distante
 *
 * @return La session ssh qui à été créée avec les différentes information données
 */
ssh_session ssh_connexion_init(const char *host, int port, const char *user, const char *password);

/**
 * @brief Vérifie la connexion avec une machine distante en essayant d'envoyer une commande (ls)
 *
 * @param session La session utilisé pour la communication
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t ssh_dry_run(ssh_session session);

/**
 * @brief Tue un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à tuer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t ssh_kill_processus(ssh_session session, int pid);

/**
 * @brief Termine proprement un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à terminer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t ssh_term_processus(ssh_session session, int pid);

/**
 * @brief Arrête un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à arrêter
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t ssh_stop_processus(ssh_session session, int pid);

/**
 * @brief Remet un processus arrêté en marche à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à remettre en marche
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t ssh_cont_processus(ssh_session session, int pid);

/**
 * @brief Redémarre un processus à distance grace à la stucture processus_t lié au processus
 *
 * @param session La session utilisé pour la communication
 * @param p Structure processus_t du processus à redémarrer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t ssh_restart_processus(ssh_session session, processus_t *p);

/*TODO : doc */
error_code_t ssh_cmd_exec(ssh_session session, char *buffer, size_t buffer_size, const char *cmd);

#endif //SSH_CONNEXION
