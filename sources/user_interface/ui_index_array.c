#include "ui_index_array.h"

#include <stdlib.h>

void ui_index_array_reset(index_array_t *array) {
	array->size = 0;
}

size_t *ui_index_array_emplace_back(index_array_t *array, size_t value) {

	if (!array) {
		return nullptr;
	}

	if (array->size >= array->capacity) {
		const size_t new_capacity = array->capacity ? array->capacity * 2 : 1;

		size_t *temp = realloc(array->data, new_capacity * sizeof(*temp));

		if (!temp) {
			return nullptr;
		}

		array->data = temp;
		array->capacity = new_capacity;
	}

	array->data[array->size++] = value;

	return &array->data[array->size - 1];	
}

void ui_index_array_free(index_array_t *array) {
        if (!array) {
			return;
		}
        free(array->data);
        array->data = nullptr;
        array->size = 0;
        array->capacity = 0;
}
