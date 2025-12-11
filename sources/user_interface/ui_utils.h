#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <stddef.h>
#include "ui.h"

void ui_utils_clamp_int(int *value, const int min_value, const int max_value);
void ui_utils_clamp_size_t(size_t *value, const size_t min_value, const size_t max_value);
void ui_utils_reset_input_buffer(user_selection_t *s);

#endif /* UI_UTILS_H */
