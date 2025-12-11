#ifndef UI_PAGE_H
#define UI_PAGE_H

#include <ncurses.h>
#include "ui_constant.h"
#include "ui.h"

void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]);

void ui_show_header(const size_t header_selected, ui_t *ui, const bool asc);

void show_help_page(ui_t *ui);

#endif /* UI_PAGE_H */
