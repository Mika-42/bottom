#include "processus_ptr_array.h"
#include "processus_signal.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void proc_ptr_array_init(processus_ptr_array_t *array) {
	if(!array) return;
	array->data = nullptr;
	array->size = 0;
	array->capacity = 0;
}
void proc_ptr_array_reset(processus_ptr_array_t *array) {
	if (array) array->size = 0;
}

error_code_t proc_ptr_array_emplace_back(processus_ptr_array_t *array, processus_t *proc) {

	if (!array || !proc) return NULLPTR_PARAMETER_ERROR;

	if (array->size >= array->capacity) {
		const size_t new_capacity = array->capacity ? array->capacity * 2 : 1;

		processus_t **temp = realloc(array->data, new_capacity * sizeof(*temp));

		if (!temp) return MEMORY_ALLOCATION_FAILED;

		array->data = temp;
		array->capacity = new_capacity;
	}

	array->data[array->size++] = proc;

	return SUCCESS;
}

void proc_ptr_array_free(processus_ptr_array_t *array) {

	if (!array) return;

	free(array->data);
	array->data = nullptr;
	array->size = 0;
	array->capacity = 0;
}

