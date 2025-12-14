#ifndef PROCESSUS_SORT_H
#define PROCESSUS_SORT_H

#include <stdlib.h>
#include <string.h>
#include "processus_array.h"

void	proc_array_sort(processus_array_t *array, proc_compare_t cmp);
int	pid_asc(const processus_t *lhs, const processus_t *rhs);
int	pid_dsc(const processus_t *lhs, const processus_t *rhs);
int	state_asc(const processus_t *lhs, const processus_t *rhs);
int	state_dsc(const processus_t *lhs, const processus_t *rhs);
int	ram_asc(const processus_t *lhs, const processus_t *rhs);
int	ram_dsc(const processus_t *lhs, const processus_t *rhs);
int	name_asc(const processus_t *lhs, const processus_t *rhs);
int	name_dsc(const processus_t *lhs, const processus_t *rhs);
int	user_asc(const processus_t *lhs, const processus_t *rhs);
int	user_dsc(const processus_t *lhs, const processus_t *rhs);
int	time_asc(const processus_t *lhs, const processus_t *rhs);
int	time_dsc(const processus_t *lhs, const processus_t *rhs);
int	cpu_asc(const processus_t *lhs, const processus_t *rhs);
int	cpu_dsc(const processus_t *lhs, const processus_t *rhs);
#endif /* PROCESSUS_SORT_H */
