#ifndef UI_H
#define UI_H
#include <ncurses.h>

void ui_init();

void ui_show_fn_cmd();

void ui_scroll(const int dx, const int dy);


#endif //UI_H
