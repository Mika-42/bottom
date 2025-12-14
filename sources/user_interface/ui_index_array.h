#ifndef UI_INDEX_ARRAY_H
#define UI_INDEX_ARRAY_H

#include <stddef.h>
#include "error.h"

typedef struct index_array_t {
	size_t *data;
	size_t size;
	size_t capacity;
} index_array_t;

void	ui_index_array_reset(index_array_t *array);
size_t*	DO_NOT_IGNORE ui_index_array_emplace_back(index_array_t *array, size_t value);
void	ui_index_array_free(index_array_t *array);

#endif /* UI_INDEX_ARRAY_H */
