#ifndef PROCESSUS_ARRAY_H
#define PROCESSUS_ARRAY_H

#include "processus.h"

typedef struct processus_array_t {
	processus_t	*data;
	size_t		size;
} processus_array_t;

typedef int (*proc_compare_t)(const processus_t *, const processus_t *);

processus_t *proc_array_get_last(processus_array_t *array);
processus_t *proc_array_emplace_back(processus_array_t *array);
void proc_array_sort(processus_array_t *array, proc_compare_t cmp);

int pid_asc(const processus_t *lhs, const processus_t *rhs);
int pid_dsc(const processus_t *lhs, const processus_t *rhs);
int state_asc(const processus_t *lhs, const processus_t *rhs);
int state_dsc(const processus_t *lhs, const processus_t *rhs);
int rss_asc(const processus_t *lhs, const processus_t *rhs);
int rss_dsc(const processus_t *lhs, const processus_t *rhs);
int name_asc(const processus_t *lhs, const processus_t *rhs);
int name_dsc(const processus_t *lhs, const processus_t *rhs);
int user_asc(const processus_t *lhs, const processus_t *rhs);
int user_dsc(const processus_t *lhs, const processus_t *rhs);

#endif //PROCESSUS_ARRAY_H
