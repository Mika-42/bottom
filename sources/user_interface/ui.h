#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>

void ui_init();

void ui_show_fn_cmd();
void ui_show_proc(const processus_array_t *array);
void ui_show_tab_header();


void update(const size_t size);
void ui_scroll(const int dx, const int dy);


#endif //UI_H
