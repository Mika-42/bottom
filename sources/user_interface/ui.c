#include "ui.h"
#include "format.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include "signal_process.h"
#include <string.h>
#include <time.h>

#include "ui_utils.h"
#include "ui_event_dispatcher.h"
#include "ui_page.h"

static WINDOW *ui_pad = nullptr;
static WINDOW *ui_footer = nullptr;
static WINDOW *ui_header = nullptr;


static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

void ui_init() {
	setlocale(LC_ALL,"");
	initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        flushinp();
        curs_set(0);
	nodelay(stdscr, TRUE);
	
	ui_pad = newpad(ui_pad_lines, ui_pad_columns);
	ui_footer = newpad(ui_footer_lines, ui_pad_columns);
	ui_header = newpad(ui_header_lines, ui_pad_columns);
}

void ui_show_header_temp(const size_t header_selected, const bool asc) {
	box(ui_header, 0, 0);

	char *arrow[header_element_count] = {" ", " ", " ", " ", " ", " ", " "};
	if (header_selected < header_element_count) arrow[header_selected] = asc ? "▲" : "▼";
        
	mvwprintw(ui_header, 0, 0, proc_array_tab_header[0]);
	mvwprintw(ui_header, 1, 0, proc_array_tab_header[1], arrow[0],arrow[1],arrow[2],arrow[3],arrow[4],arrow[5],arrow[6]);
	mvwprintw(ui_header, 2, 0, proc_array_tab_header[2]);

}

void ui_show_proc(const processus_array_t *array,  user_selection_t *s) {
	werase(ui_pad);

	if (!array) return;
	size_t filter_index = 0;
	for (size_t i=0; i<array->size; ++i) {
		double ram;
		const char *unit = format_ram(array->data[i].ram, &ram);

		char buf[32];
		format_time(array->data[i].start_time, buf, 32);

		if (i == s->selected) wattron(ui_pad, A_REVERSE);

		if (s->search_mode) {
			if (strncmp(array->data[i].name, s->input, s->input_length) == 0) {
				mvwprintw(ui_pad, filter_index++, 0, separator, 
						array->data[i].pid, 
						array->data[i].user, 
						array->data[i].name, 
						format_state(array->data[i].state),
						ram, unit, 
						100.00 //fictional data TODO implement CPU usage
						, buf
					 );
			}
		} else {	
			mvwprintw(ui_pad, i, 0, separator, 
					array->data[i].pid, 
					array->data[i].user, 
					array->data[i].name, 
					format_state(array->data[i].state),
					ram, unit, 
					100.00 //fictional data TODO implement CPU usage
					, buf
				 );
		}
		if (i == s->selected) wattroff(ui_pad, A_REVERSE);	
	}

	if (s->search_mode) {
		for (; filter_index<(size_t)getmaxy(stdscr); ++filter_index) {
			mvwprintw(ui_pad, filter_index, 0, "┃ %-10s ┃ %-24s ┃ %-32s ┃ %-10s ┃ %-10s ┃ %-7s ┃  %-18s ┃", "", "", "", "", "", "", "");
		}
	}
}

void ui_update(const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;

	ui_utils_clamp_int(&ui_scroll_y, 0, (int)size - view_height);

	pnoutrefresh(ui_header, 0, ui_scroll_x, 0, 0, ui_header_lines - 1, terminal_width - 1);
	pnoutrefresh(ui_pad, ui_scroll_y, ui_scroll_x, ui_header_lines, 0, terminal_height - ui_footer_lines - 1, terminal_width - 1);
	pnoutrefresh(ui_footer, 0, ui_scroll_x, terminal_height - ui_footer_lines, 0, terminal_height - 1, terminal_width - 1);

	doupdate();
}

void ui_scroll(const int dx, const size_t selected) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	ui_scroll_x += dx;
	ui_utils_clamp_int(&ui_scroll_x, 0, ui_pad_columns - terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;
	const int hi = ui_scroll_y;
	const int lo = hi + view_height - 1;

	if ((int)selected < hi) ui_scroll_y = selected;
	else if ((int)selected > lo) ui_scroll_y = selected - view_height + 1;

	ui_utils_clamp_int(&ui_scroll_y, 0,  ui_pad_lines - view_height);

}

int global_event_dispatcher(const int ch, const processus_array_t *array, user_selection_t *s) {

	if (ch == KEY_LEFT) return -1;
	if (ch == KEY_RIGHT) return 1;

	if (!s->help) {
		switch(ch) {
			case '\t':
				if (!s->search_mode) {
					s->header_selected = (s->header_selected + 1) % header_element_count;
				}
				break;

			case '\n': 
				if (!s->search_mode) {
					s->asc = !s->asc; 
				} 
				break;

			case KEY_UP: 
				if (s->selected != 0) {
					--s->selected; 
				} 
				break;
			case KEY_DOWN: 
				if (s->selected < array->size - 1) {
					++s->selected; 
				}
				break;
		}
	}
	return 0;
}

error_code_t ui_main(const processus_array_t array[], user_selection_t *user_selection) {

	ui_init();

	for (; ; ) {

		auto machine = &array[user_selection->machine_selected];
		auto proc = &machine->data[user_selection->selected];

		const int ch = getch();

		if (user_selection->help) { } else {	
		}

		if (ch == KEY_F(9)) {
			endwin();
			return SUCCESS;
		}

		if(user_selection->help) {
			ui_event_dispatcher_help(ch, user_selection);

			show_help_page(ui_pad, ui_header, ui_footer);
		} else {
			if (user_selection->search_mode) {
				ui_event_dispatcher_search(ch, user_selection);

				ui_show_array(ui_footer, proc_array_search_bar);
				mvwprintw(ui_footer, 1, 49, "%-71.71s", user_selection->input);

			} else {
				auto callback = ui_event_dispatcher_normal(&machine, ch, user_selection);
				if (callback) callback(proc); 
				ui_show_array(ui_footer, proc_array_function_command);
			}

			ui_show_header_temp(user_selection->header_selected, user_selection->asc);
			ui_show_proc(machine, user_selection);
		}

		const int scroll_factor = global_event_dispatcher(ch, machine, user_selection);
		ui_scroll(scroll_factor, user_selection->selected);
		ui_update(machine->size);

		struct timespec ts = {
			.tv_sec = 0,
			.tv_nsec = 1'000'000'000 / 40 // 1/40 seconde
		};

		nanosleep(&ts, nullptr);
	}
}
