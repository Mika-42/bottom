#include "processus_sort.h"

void proc_array_sort(processus_array_t *array, proc_compare_t cmp) {

	if (!array || !array->data || !cmp) {
		return;
	}

	qsort(array->data, array->size, sizeof(processus_t),
			(int (*)(const void*, const void*))cmp);
}

int pid_asc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->pid > rhs->pid) - (lhs->pid < rhs->pid);
}

int pid_dsc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->pid < rhs->pid) - (lhs->pid > rhs->pid);
}

int state_asc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->state > rhs->state) - (lhs->state < rhs->state);
}

int state_dsc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->state < rhs->state) - (lhs->state > rhs->state);
}

int ram_asc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->ram > rhs->ram) - (lhs->ram < rhs->ram);
}

int ram_dsc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->ram < rhs->ram) - (lhs->ram > rhs->ram);
}

int name_asc(const processus_t *lhs, const processus_t *rhs) {
	return strcmp(lhs->name, rhs->name);
}

int name_dsc(const processus_t *lhs, const processus_t *rhs) {
	return strcmp(rhs->name, lhs->name);
}

int user_asc(const processus_t *lhs, const processus_t *rhs) {
	return strcmp(lhs->user, rhs->user);
}

int user_dsc(const processus_t *lhs, const processus_t *rhs) {
	return strcmp(rhs->user, lhs->user);
}

int time_asc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->start_time < rhs->start_time) - (lhs->start_time > rhs->start_time);
}

int time_dsc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->start_time > rhs->start_time) - (lhs->start_time < rhs->start_time);
}

int cpu_asc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->cpu_usage > rhs->cpu_usage) - (lhs->cpu_usage < rhs->cpu_usage);
}

int cpu_dsc(const processus_t *lhs, const processus_t *rhs) {
	return (lhs->cpu_usage < rhs->cpu_usage) - (lhs->cpu_usage > rhs->cpu_usage);
}
