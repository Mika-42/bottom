#ifndef PROCESSUS_ARRAY_H
#define PROCESSUS_ARRAY_H

#include <time.h>
#include "processus.h"

/**
 * @brief Tableau de processus
 */
typedef struct processus_array_t {
	processus_t	*data;		/**< Tableau de processus */
	size_t		size;		/**< Nombre de processus dans le tableau */
	size_t		capacity;	/**< Capacité maximum d'élément du tableau (taille alloué) */
	time_t		cpu_tick;	/**< Temps d’activité des CPU */
	time_t		boot_time;	/**< Temps écoulé depuis le démmarage de la machine */
} processus_array_t;

typedef int (*proc_compare_t)(const processus_t *, const processus_t *);
typedef bool(*proc_predicate_t)(processus_t*);

void		proc_array_init(processus_array_t *array);
[[nodiscard]] processus_t* proc_array_get_last(processus_array_t *array);
[[nodiscard]] processus_t* proc_array_emplace_back(processus_array_t *array);
void		proc_array_free(processus_array_t *array);
[[nodiscard]] error_code_t proc_array_update(processus_array_t *array);
[[nodiscard]] processus_t* proc_array_find_by_pid(const processus_array_t *array, const pid_t pid);
void		proc_array_reset(processus_array_t *array);
void		proc_array_remove_if(processus_array_t *array, proc_predicate_t pred);
[[nodiscard]] error_code_t proc_array_get_cpu(const processus_array_t *prev_array, processus_array_t *current_array);

#endif //PROCESSUS_ARRAY_H
