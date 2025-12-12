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

static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

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

void ui_update(ui_t *ui, const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;

	ui_utils_clamp_int(&ui_scroll_y, 0, (int)size - view_height);

	pnoutrefresh(ui->header, 0, ui_scroll_x, 0, 0, ui_header_lines - 1, terminal_width - 1);
	pnoutrefresh(ui->pad, ui_scroll_y, ui_scroll_x, ui_header_lines, 0, terminal_height - ui_footer_lines - 1, terminal_width - 1);
	pnoutrefresh(ui->footer, 0, ui_scroll_x, terminal_height - ui_footer_lines, 0, terminal_height - 1, terminal_width - 1);

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
