#ifndef UI_PAGE_H
#define UI_PAGE_H

#include <ncurses.h>
#include "ui_constant.h"

void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]);

void show_help_page(WINDOW *pad, WINDOW *header, WINDOW *footer);

#endif /* UI_PAGE_H */
