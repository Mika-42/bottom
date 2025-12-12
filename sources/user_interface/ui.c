#include "ui.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include <time.h>

#include "processus_signal.h"
#include "ui_utils.h"
#include "ui_event_dispatcher.h"
#include "ui_page.h"

ui_t ui = {0};

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

	ui.pad = newpad(ui_pad_lines, ui_pad_columns);
	ui.footer = newpad(ui_footer_lines, ui_pad_columns);
	ui.header = newpad(ui_header_lines, ui_pad_columns);
}

void ui_update(const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;

	ui_utils_clamp_int(&ui_scroll_y, 0, (int)size - view_height);

	pnoutrefresh(ui.header, 0, ui_scroll_x, 0, 0, ui_header_lines - 1, terminal_width - 1);
	pnoutrefresh(ui.pad, ui_scroll_y, ui_scroll_x, ui_header_lines, 0, terminal_height - ui_footer_lines - 1, terminal_width - 1);
	pnoutrefresh(ui.footer, 0, ui_scroll_x, terminal_height - ui_footer_lines, 0, terminal_height - 1, terminal_width - 1);

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
	const int lo = hi + view_height;

	if ((int)selected < hi) ui_scroll_y = selected;
	else if ((int)selected >= lo) ui_scroll_y = selected - view_height + 1;

	ui_utils_clamp_int(&ui_scroll_y, 0,  ui_pad_lines - view_height);
}

error_code_t ui_main(const processus_array_t array[], user_selection_t *s) {

	ui_init();

	constexpr struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 1'000'000'000 / 40 // 1/40 seconde
	};

	size_t select = 0;
	
	for (;;) {

		auto machine = &array[s->machine_selected];

		const int ch = getch();

		if (ch == KEY_F(9)) {
			endwin();
			return SUCCESS;
		}

		if(s->help) {	
			ui_scroll_y = 0;       
			ui_event_dispatcher_help(ch, &ui, s);
			select = 0;
		} else {
			if (s->search_mode) {

				ui_event_dispatcher_search(array, ch, &ui, s);


			} else {
				ui_event_dispatcher_normal(array, ch, &ui, s);
			}

			select = s->selected;
		}

		const int scroll_factor = ui_event_dispatcher_global(ch);
		ui_scroll(scroll_factor, select);
		ui_update(machine->size);

		nanosleep(&ts, nullptr);
	}
}
