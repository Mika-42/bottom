#include "ui_filter.h"
#include <string.h>

void ui_filter_proc(const processus_array_t *array, user_selection_t *s) {
	ui_index_array_reset(&s->indices);

	for (size_t i=0; i<array->size; ++i) {
                if (strncmp(array->data[i].name, s->input, s->input_length) == 0 || s->input[0] == '\0') {

			ui_index_array_emplace_back(&s->indices, i);
		}
        }
}

