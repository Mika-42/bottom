#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <stddef.h>

void ui_utils_clamp_int(int *value, const int min_value, const int max_value);
void ui_utils_clamp_size_t(size_t *value, const size_t min_value, const size_t max_value);

#endif /* UI_UTILS_H */
