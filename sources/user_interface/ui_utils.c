#include "ui_utils.h"
#include <string.h>

void ui_utils_clamp_int(int *value, const int min_value, const int max_value) {
	
	if(min_value > max_value) return;

	if (*value < min_value) {
		*value = min_value;
	} else if (*value > max_value) {
		*value = max_value;
	}
}

void ui_utils_clamp_size_t(size_t *value, const size_t min_value, const size_t max_value) {

	if(min_value > max_value) return;
	
	if (*value < min_value) {
		*value = min_value;
	} else if (*value > max_value) {
		*value = max_value;
	}
}

void ui_utils_reset_input_buffer(user_selection_t *s) {
	memset(s->input, 0, sizeof(s->input));
	s->input_length = 0;
}
