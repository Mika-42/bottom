#ifndef SSH_CONNEXION
#define SSH_CONNEXION

#include <libssh/libssh.h>
#include "error.h"
#include "processus.h"

/**
 * @brief Permet de see déconnecter, fermer et liberer une session ssh
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
error_code_t ssh_dry_run(ssh_session session);

/**
 * @brief Tue un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à tuer
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_kill_processus(ssh_session session, int pid);

/**
 * @brief Termine proprement un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à terminer
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_term_processus(ssh_session session, int pid);

/**
 * @brief Arrête un processus à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à arrêter
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_stop_processus(ssh_session session, int pid);

/**
 * @brief Remet un processus arrêté en marche à distance grace à son pid
 *
 * @param session La session utilisé pour la communication
 * @param pid Le pid du processus à remettre en marche
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_cont_processus(ssh_session session, int pid);

/**
 * @brief Redémarre un processus à distance grace à la stucture processus_t lié au processus
 *
 * @param session La session utilisé pour la communication
 * @param p Structure processus_t du processus à redémarrer
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_restart_processus(ssh_session session, processus_t *p);

/**
 * @brief Récupère dans un buffer de taille fixe le contenue d'un fichier de /proc/ d'une machine distante
 *
 * @param session La session ssh utilisé pour la communication
 * @param buffer Le buffer de taille fixe dans lequel sera écrit le contenue du fichier
 * @param buffer_size La taille fixe maximum du buffer
 * @param file Le fichier qui contient ce que le buffer doit contenir en sortie
 * 
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_get_file(ssh_session session, char *buffer, size_t buffer_size, const char *file);

/**
 * @brief Récupère dans un buffer de taille fixe l'executable d'un processus
 *
 * @param session La session ssh utilisé pour la communication
 * @param buffer Le buffer de taille fixe dans lequel sera écrit l'executable du processus donné
 * @param buffer_size La taille fixe maximum du buffer
 * @param p La structure processus_t du processus pour qui on cherche l'executable
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_get_exe(ssh_session session, char *buffer, size_t buffer_size, processus_t *p);

/**
 * @brief Met dans une liste de pid_t, tous les pids d'une machine distant et dans count, le nombre de pid entré dans la liste
 *
 * @param session La session ssh utilisé pour la communication
 * @param pid_list La liste de type pid_t de taille fixe dans laquelle sera entré tout les pid de la machine distant (si la taille est suffisante)
 * @param size_list La taille fixe maximum de la liste de pid
 * @param count Un entier dans lequel sera écrit le nombre de pid qui ont été entré dans la liste de pid
 *
 * @return Un code d'erreur défini dans error.h
 */
error_code_t ssh_get_pid_list(ssh_session session, pid_t *pid_list, size_t size_list, size_t *count);

#endif //SSH_CONNEXION
