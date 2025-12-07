#ifndef PROCESSUS_ARRAY_H
#define PROCESSUS_ARRAY_H

#include "processus.h"

typedef struct processus_array_t {
	processus_t	*data;
	size_t		size;
	size_t		capacity;
	long		cpu_tick;
} processus_array_t;

typedef int (*proc_compare_t)(const processus_t *, const processus_t *);
typedef bool(*proc_predicate_t)(processus_t*);

processus_t	*proc_array_get_last(processus_array_t *array);
processus_t	*proc_array_emplace_back(processus_array_t *array);
void		proc_array_free(processus_array_t *array);
error_code_t	proc_array_update(processus_array_t* array);
processus_t	*proc_array_find_by_pid(processus_array_t *array, const pid_t pid);
void		proc_array_remove_if(processus_array_t *array, proc_predicate_t pred);

void		proc_array_sort(processus_array_t *array, proc_compare_t cmp);
int		pid_asc(const processus_t *lhs, const processus_t *rhs);
int		pid_dsc(const processus_t *lhs, const processus_t *rhs);
int		state_asc(const processus_t *lhs, const processus_t *rhs);
int		state_dsc(const processus_t *lhs, const processus_t *rhs);
int		ram_asc(const processus_t *lhs, const processus_t *rhs);
int		ram_dsc(const processus_t *lhs, const processus_t *rhs);
int		name_asc(const processus_t *lhs, const processus_t *rhs);
int		name_dsc(const processus_t *lhs, const processus_t *rhs);
int		user_asc(const processus_t *lhs, const processus_t *rhs);
int		user_dsc(const processus_t *lhs, const processus_t *rhs);

#endif //PROCESSUS_ARRAY_H
