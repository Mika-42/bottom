#ifndef PROCESSUS_SORT_H
#define PROCESSUS_SORT_H

#include <stdlib.h>
#include <string.h>
#include "processus_array.h"

/**
 * @brief Trie un tableau de processus
 *
 * @param array Tableau à trier
 * @param cmp fonction de comparaison utilisée pour le trie
 */
void proc_array_sort(processus_array_t *array, proc_compare_t cmp);

/**
 * @brief Fonction de tri par pid croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int pid_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par pid décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int pid_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par état croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int state_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par état décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int state_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par consommation de ram croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int ram_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par consommation de ram décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int ram_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par nom croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int name_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par nom décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int name_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par utilisateur croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int user_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par utilisateur décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int user_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par durée de lancement croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int time_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par durée de lancement décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int time_dsc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par consommation cpu croissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int cpu_asc(const processus_t *lhs, const processus_t *rhs);

/**
 * @brief Fonction de tri par consommation cpu décroissant
 *
 * @param lhs Processus 1
 * @param rhs Processus 2
 *
 * return Un entier <0 si lhs doit être avant rhs et >0 si rhs doit être avant lhs
 */
[[nodiscard]] int cpu_dsc(const processus_t *lhs, const processus_t *rhs);

#endif /* PROCESSUS_SORT_H */
