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


void ui_init(ui_t *ui) {
	setlocale(LC_ALL,"");
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	flushinp();
	curs_set(0);
	nodelay(stdscr, TRUE);

	ui->pad = newpad(ui_pad_lines, ui_pad_columns);
	ui->footer = newpad(ui_footer_lines, ui_pad_columns);
	ui->header = newpad(ui_header_lines, ui_pad_columns);
}

void ui_deinit(ui_t *ui) {
	endwin();
	delwin(ui->pad);
	delwin(ui->footer);
	delwin(ui->header);
}

void ui_update(ui_t *ui, const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;

	ui_utils_clamp_int(&ui->ui_scroll_y, 0, (int)size - view_height);

	pnoutrefresh(ui->header, 0, ui->ui_scroll_x, 0, 0, ui_header_lines - 1, terminal_width - 1);
	pnoutrefresh(ui->pad, ui->ui_scroll_y, ui->ui_scroll_x, ui_header_lines, 0, terminal_height - ui_footer_lines - 1, terminal_width - 1);
	pnoutrefresh(ui->footer, 0, ui->ui_scroll_x, terminal_height - ui_footer_lines, 0, terminal_height - 1, terminal_width - 1);

	doupdate();
}

void ui_scroll(ui_t *ui, const int dx, const size_t selected) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);
	int max_scroll_x = 0;
	if ((int)ui_pad_columns > terminal_width) max_scroll_x = ui_pad_columns - terminal_width;
	ui->ui_scroll_x += dx;
	
	ui_utils_clamp_int(&ui->ui_scroll_x, 0, max_scroll_x);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;
	const int hi = ui->ui_scroll_y;
	const int lo = hi + view_height;

	if ((int)selected < hi) ui->ui_scroll_y = selected;
	else if ((int)selected >= lo) ui->ui_scroll_y = selected - view_height + 1;

	ui_utils_clamp_int(&ui->ui_scroll_y, 0,  ui_pad_lines - view_height);
}

void ui_select(ui_t *ui, user_selection_t *s) {

	if (s->indices.size != 0) {
		if (s->selected > 0) mvwchgat(ui->pad, s->selected - 1, 0, -1, A_NORMAL, 0, nullptr);
		mvwchgat(ui->pad, s->selected, 0, -1, A_REVERSE, 0, nullptr);
		if (s->selected < s->indices.size) mvwchgat(ui->pad, s->selected + 1, 0, -1, A_NORMAL, 0, nullptr);
	}
}
