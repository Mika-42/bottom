#include "ui_page.h"
#include <string.h>
#include <unistd.h>
#include "ui_format.h"
#include "ui_filter.h"

void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]) {	
	box(win, 0, 0);

	for (size_t i=0; i<ui_footer_lines; ++i) {
		mvwprintw(win, i, 0, array[i]);
	}
}

void ui_show_header(const size_t header_selected, ui_t *ui, const bool asc) {
	box(ui->header, 0, 0);

	char *arrow[header_element_count] = {" ", " ", " ", " ", " ", " ", " "};
	if (header_selected < header_element_count) {
		arrow[header_selected] = asc ? "▲" : "▼";
	}

	mvwprintw(ui->header, 0, 0, proc_array_tab_header[0]);
	mvwprintw(ui->header, 1, 0, proc_array_tab_header[1]);
	mvwprintw(ui->header, 2, 0, proc_array_tab_header[2]);
	mvwprintw(ui->header, 3, 0, proc_array_tab_header[3], arrow[0], arrow[1], arrow[2], arrow[3], arrow[4], arrow[5], arrow[6]);
	mvwprintw(ui->header, 4, 0, proc_array_tab_header[4]);
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

error_code_t ui_show_proc(const processus_array_t *array, ui_t *ui, user_selection_t *s) {

	werase(ui->pad);

	if (!array) {
		return NULLPTR_PARAMETER_ERROR;
	}

	size_t i = 0;

	const error_code_t err = ui_filter_proc(array, s);
	if (err != SUCCESS) {
		return err;
	}

	for (; i<s->indices.size; ++i) {
		size_t index = s->indices.data[i];
		
		double ram;
		const char *unit = ui_format_ram(array->data[index].ram, &ram);

		char buf[16];
		const long ticks_per_sec = sysconf(_SC_CLK_TCK);
		const time_t start_time = array->boot_time + array->data[index].start_time / ticks_per_sec;
		const time_t now = time(nullptr);
		ui_format_time(now - start_time, buf, 16);

		mvwprintw(ui->pad, i, 0, separator, 
				array->data[index].pid, 
				array->data[index].user, 
				array->data[index].name, 
				ui_format_state(array->data[index].state),
				ram, unit, 
				array->data[index].cpu_usage,
				buf
			 );
	}

	if (s->mode == SEARCH) {
		for (; i<(size_t)getmaxy(stdscr); ++i) {
			mvwprintw(ui->pad, i, 0, empty_separator, "", "", "", "", "", "", "");
		}
	}
	
	return SUCCESS;
}
