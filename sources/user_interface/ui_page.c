#include "ui_page.h"

void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]) {	
	box(win, 0, 0);

	for(size_t i = 0; i < ui_footer_lines; ++i) {
		mvwprintw(win, i, 0, array[i]);
	}
}

void ui_show_header(const size_t header_selected, ui_t *ui, const bool asc) {
	box(ui->header, 0, 0);

	char *arrow[header_element_count] = {" ", " ", " ", " ", " ", " ", " "};
	if (header_selected < header_element_count) arrow[header_selected] = asc ? "▲" : "▼";
        
	mvwprintw(ui->header, 0, 0, proc_array_tab_header[0]);
	mvwprintw(ui->header, 1, 0, proc_array_tab_header[1], arrow[0],arrow[1],arrow[2],arrow[3],arrow[4],arrow[5],arrow[6]);
	mvwprintw(ui->header, 2, 0, proc_array_tab_header[2]);
}

void show_help_page(ui_t *ui) {

	werase(ui->pad);	

	ui_show_array(ui->header, proc_array_help_header);

	size_t i = 0;
	for (; i<ui_help_lines; ++i) {
		mvwprintw(ui->pad, i, 0, proc_array_help[i]);
	}

	for (; i<(size_t)getmaxy(stdscr); ++i) {
		mvwprintw(ui->pad, i, 0, proc_array_help[ui_help_lines - 1]);
	}

	ui_show_array(ui->footer, proc_array_help_footer);
}
