#include "ui_utils.h"

void ui_utils_clamp_int(int *value, const int min_value, const int max_value) {

	if(*value < min_value) {
		*value = min_value;
	} else if(*value > max_value) {
		*value = max_value;
	}
}
