#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>

void ui_init();

void ui_show_footer(const char **array);
void ui_show_proc(const processus_array_t *array, const size_t selected);
void ui_show_header(const size_t header_selected, const bool asc);

void constrain_strict(int *value, const int min, const int max);

void ui_update(const size_t size);
void ui_scroll(const int dx, const size_t selected);


#endif //UI_H
