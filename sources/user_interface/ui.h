#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>

void ui_init();

void ui_show_fn_cmd();
void ui_show_search_bar();
void ui_show_proc(const processus_array_t *array, const size_t selected);
void ui_show_tab_header(const size_t header_selected, const bool asc);

void constrain_strict(int *value, const int min, const int max);

void update(const size_t size);
void ui_scroll(const int dx, const size_t selected);


#endif //UI_H
