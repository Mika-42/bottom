#ifndef UI_INDEX_ARRAY_H
#define UI_INDEX_ARRAY_H

#include <stddef.h>

typedef struct index_array_t {
	size_t *data;
	size_t size;
	size_t capacity;
} index_array_t;

void	ui_index_array_reset(index_array_t *array);
size_t	*ui_index_array_emplace_back(index_array_t *array, size_t value);
#endif /* UI_INDEX_ARRAY_H */
