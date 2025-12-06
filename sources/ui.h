, const processus_array_t *arra*****y#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>

void ui_init();

void ui_show_fn_cmd();
void ui_show_proc(const processus_array_t *array);

void ui_scroll(const int dx, const int dy, const processus_array_t *array);


#endif //UI_H
