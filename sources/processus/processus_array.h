#ifndef PROCESSUS_ARRAY_H
#define PROCESSUS_ARRAY_H

#include <time.h>
#include "processus.h"

/**
 * @brief Tableau de processus
 */
typedef struct processus_array_t {
    processus_t *data;      /**< Tableau de processus */
    size_t      size;       /**< Nombre de processus dans le tableau */
    size_t      capacity;   /**< Capacité maximum d'élément du tableau (taille alloué) */
    time_t      cpu_tick;   /**< Temps d’activité des CPU */
    time_t      boot_time;  /**< Temps écoulé depuis le démmarage de la machine */
} processus_array_t;

typedef int (*proc_compare_t)(const processus_t *, const processus_t *);
typedef bool (*proc_predicate_t)(processus_t *);
/**
 * @brief Initialise un tableau de processus
 *
 * @param array Le tableau de processus
 */
void        proc_array_init(processus_array_t *array);

/**
 * @brief Permet de récupérer le dernier processus d'un tableau
 *
 * @param array Un tableau de processus
 *
 * @return Le dernier processus du tableau donné
 */
[[nodiscard]] processus_t *proc_array_get_last(processus_array_t *array);

/**
 * @brief Réallou de la mémoire en plus a un tableau de processus
 *
 * @param array Un tableau de processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] processus_t *proc_array_emplace_back(processus_array_t *array);

/**
 * @brief Libère la mémoire alloué à un tableau de processus
 *
 * @param array Un tableau de processus
 */
void        proc_array_free(processus_array_t *array);

/**
 * @brief Met à jour un tableau de processus avec les processus de la machine local
 *
 * @param array Un tableau de processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_array_update(processus_array_t *array);

/**
 * @brief Permet de récupérer la structure d'un processus à partir de son pid
 *
 * @param array Un tableau de processus
 * @param pid Pid du processus à chercher
 *
 * @return La structure du processus cherché
 */
[[nodiscard]] processus_t *proc_array_find_by_pid(const processus_array_t *array, const pid_t pid);

/**
 * @brief Remet un tableau de processus à zero
 *
 * @param array Un tableau de processus
 */
void        proc_array_reset(processus_array_t *array);

/**
 * @brief Retire tout les processus pour lesquels une fonction renvoie vrai
 *
 * @param array Un tableau de processus
 * @param pred La fonction qui vérifie si on doit supprimer un processus
 */
void        proc_array_remove_if(processus_array_t *array, proc_predicate_t pred);

/**
 * @brief Compare la consommation cpu de chaque processus d'un tableau de processus entre deux instance
 *
 * @param prev_array La première instance du tableau de processus
 * @param cuurent_array La second instance du tableau de processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_array_get_cpu(const processus_array_t *prev_array, processus_array_t *current_array);

#endif //PROCESSUS_ARRAY_H
