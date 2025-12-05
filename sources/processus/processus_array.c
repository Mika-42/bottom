#include "processus_array.h"
#include <stdlib.h>
#include <string.h>

processus_t *proc_array_get_last(processus_array_t *array) {
	return (!array || array->size == 0 || !array->data) ? 
		nullptr : &array->data[array->size - 1];
}

processus_t *proc_array_emplace_back(processus_array_t *array) {

	if(!array) return nullptr;

	const size_t new_size = array->size + 1;

	processus_t *temp = realloc(array->data, new_size * sizeof(*temp));

	if(!temp) return nullptr;
	
	array->data = temp;
	array->size = new_size;
	
	return proc_array_get_last(array);	
}

void remove_if(processus_array_t *array, bool(*predicate)(processus_t*)) {
	
	size_t write_index = 0;

	for(size_t i = 0; i < array->size; ++i) {
		processus_t *current = &(array->data[i]);
		
		///> Ici on écrase les valeurs à retirer avec les valeurs que l'on souhaite garder
		if(!predicate(current)) array->data[write_index++] = *current;
	}
	array->size = write_index;
}

void proc_array_sort(processus_array_t *array, proc_compare_t cmp) {

	if (!array || !array->data || !cmp) return;
	
	qsort(array->data, array->size, sizeof(processus_t),
        (int (*)(const void*, const void*))cmp);
}


int pid_asc(const processus_t *lhs, const processus_t *rhs) {
	return lhs->pid - rhs->pid;
}

int pid_dsc(const processus_t *lhs, const processus_t *rhs) {
	return rhs->pid - lhs->pid;
}

int state_asc(const processus_t *lhs, const processus_t *rhs) {
	return lhs->state - rhs->state;
}

int state_dsc(const processus_t *lhs, const processus_t *rhs) {
	return rhs->state - lhs->state;
}

int rss_asc(const processus_t *lhs, const processus_t *rhs) {
	return lhs->ram_rss - rhs->ram_rss;
}

int rss_dsc(const processus_t *lhs, const processus_t *rhs) {
	return rhs->ram_rss - lhs->ram_rss;
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


