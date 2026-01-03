#ifndef PROCESSUS_SIGNAL_H
#define PROCESSUS_SIGNAL_H

#include "processus.h"
#include "error.h"

/**
 * @brief Vérifie qu'un pid existe
 *
 * @param pid Pid à vérifier
 *
 * @return Un booléen
 */
[[nodiscard]] bool pid_exists(pid_t pid);

/**
 * @brief Vérifie qu'un pid n'existe pas
 *
 * @param pid Pid à vérifier
 *
 * @return Un booléen
 */
[[nodiscard]] bool pid_does_not_exists(pid_t pid);

/**
 * @brief Tue un processus
 *
 * @param p Processus à tuer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t proc_kill(processus_t *p);

/**
 * @brief Met en pause un processus
 *
 * @param p Processus à mettre en pause
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t proc_stop(processus_t *p);

/**
 * @brief Relance un processus mis en pause
 *
 * @param p Processus à relancer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t proc_cont(processus_t *p);

/**
 * @brief Termine un processus
 *
 * @param p Processus à terminer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t proc_term(processus_t *p);

/**
 * @brief Redémmare un processus
 *
 * @param p Processus à redémmarer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t proc_restart(processus_t *p);

/**
 * @brief Tue les enfants d'un processus
 *
 * @param p Processus à qui tuer les enfants
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t kill_children(processus_t *p);

/**
 * @brief Attend qu'un processus s'arrête proprement
 *
 * @param pid Pid du processus à attendre
 * @param timeout_ms Durée maximum à attendre en ms avant de sortir de l'attente
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t wait_for_exit_with_timeout(pid_t pid, int timeout_ms);

#endif //PROCESSUS_SIGNAL_H
