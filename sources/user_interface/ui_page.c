#include "ui_page.h"
#include <string.h>
#include "ui_format.h"

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

void ui_show_proc(const processus_array_t *array, ui_t *ui, user_selection_t *s) {

	werase(ui->pad);

	if (!array) return;
	size_t filter_index = 0;

	for (size_t i=0; i<array->size; ++i) {

		if (s->search_mode && strncmp(array->data[i].name, s->input, s->input_length) != 0) continue;
		/**/
		double ram;
		const char *unit = ui_format_ram(array->data[i].ram, &ram);

		char buf[32];
		ui_format_time(array->data[i].start_time, buf, 32);

		mvwprintw(ui->pad, filter_index++, 0, separator, 
				array->data[i].pid, 
				array->data[i].user, 
				array->data[i].name, 
				ui_format_state(array->data[i].state),
				ram, unit, 
				100.00 //fictional data TODO implement CPU usage
				, buf
			 );
	}

	if (s->search_mode) {
		for (; filter_index<(size_t)getmaxy(stdscr); ++filter_index) {
			mvwprintw(ui->pad, filter_index, 0, empty_separator, "", "", "", "", "", "", "");
		}
	}
	
	mvwchgat(ui->pad, s->selected, 0, -1, A_REVERSE, 0, NULL);
}
