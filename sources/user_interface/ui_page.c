#include "ui_page.h"

void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]) {	
	box(win, 0, 0);

	for(size_t i = 0; i < ui_footer_lines; ++i) {
		mvwprintw(win, i, 0, array[i]);
	}
}

void show_help_page(WINDOW *pad, WINDOW *header, WINDOW *footer) {

	werase(pad);	

	ui_show_array(header, proc_array_help_header);

	size_t i = 0;
	for (; i<ui_help_lines; ++i) {
		mvwprintw(pad, i, 0, proc_array_help[i]);
	}

	for (; i<(size_t)getmaxy(stdscr); ++i) {
		mvwprintw(pad, i, 0, proc_array_help[ui_help_lines - 1]);
	}

	ui_show_array(footer, proc_array_help_footer);
}
