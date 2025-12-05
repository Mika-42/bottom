#include "processus_array.h"
#include <stdlib.h>
#include <string.h>

processus_t *proc_array_get_last(processus_array_t *array) {
	return (!array || array->size == 0 || !array->data) ? 
		nullptr : &array->data[array->size - 1];
}

processus_t *proc_array_emplace_back(processus_array_t *array) {

	if(!array) return nullptr;

	if(array->size >= array->capacity) {
		const size_t new_capacity = array->capacity ? array->capacity * 2 : 1;
		 
		processus_t *temp = realloc(array->data, new_capacity * sizeof(*temp));
		
		if(!temp) return nullptr;
		
		array->data = temp;
		array->capacity = new_capacity;
	}

	array->data[array->size++] = (processus_t){0};

	return proc_array_get_last(array);	
}

void proc_array_remove_if(processus_array_t *array, bool(*predicate)(processus_t*)) {
	
	//iterators
	processus_t *write = array->data;
	processus_t *read = array->data;
	processus_t *end = array->data + array->size;

	for(; read != end; ++read) {
		if(!predicate(read)) *write++ = *read;
	}

	array->size = (size_t)(write - array->data);
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


