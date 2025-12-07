#include "ui.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>

constexpr size_t header_element_count = 7;

static WINDOW *ui_pad = nullptr;
static WINDOW *ui_footer = nullptr;
static WINDOW *ui_header = nullptr;

static int ui_pad_lines = 5000;
static int ui_pad_columns = 134;

constexpr int ui_footer_lines = 3;
constexpr int ui_header_lines = 3;

static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

const char *proc_array_function_command[] = {
	"┣━━━━━━━━━━━┳┻━━━━━━━━━━━━━┳━━━━━━━━━━━━┻━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━┻━━┳━━━━━━━━━┻━━━━━━┳━━━━━┻━━━━━┳━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] help ┃ [F2] page -> ┃ [F3] page <- ┃ [F4] search ┃ [F5] pause/continue ┃ [F6] terminate ┃ [F7] kill ┃ [F8] reload ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_search_bar[] = {
	"┣━━━━━━━━━━━━┻┳━━━━━━━━━━━━┳━━━━━━━━━━━┳┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] cancel ┃ [F2] Prev  ┃ [F3] Next ┃ Search:                                                                         ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_tab_header[] = {
        "┏━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓",
        "┃ PID      %s ┃ User                   %s ┃ Name                           %s ┃ State    %s ┃ RAM      %s ┃ CPU   %s ┃ Time (hh:mm:ss)   %s ┃",
        "┣━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━┫",
};

const char* separator = "┃ %-10d ┃ %-24.23s ┃ %-32.31s ┃ %-10s ┃ %-6.1f %s ┃ %-6.1f%% ┃  %-18s ┃";

void ui_init()
{
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

void ui_show_fn_cmd()
{	
	werase(ui_footer);
	box(ui_footer, 0, 0);
	
	mvwprintw(ui_footer, 0, 0, proc_array_function_command[0]);
	mvwprintw(ui_footer, 1, 0, proc_array_function_command[1]);
	mvwprintw(ui_footer, 2, 0, proc_array_function_command[2]);

	wrefresh(ui_footer);
}

void ui_show_search_bar()
{	
	werase(ui_footer);
	box(ui_footer, 0, 0);
	
	mvwprintw(ui_footer, 0, 0, proc_array_search_bar[0]);
	mvwprintw(ui_footer, 1, 0, proc_array_search_bar[1]);
	mvwprintw(ui_footer, 2, 0, proc_array_search_bar[2]);

	wrefresh(ui_footer);
}

void ui_show_tab_header(const size_t header_selected, const bool asc)
{
	werase(ui_header);
	box(ui_header, 0, 0);

	char* arrow[header_element_count] = {" ", " ", " ", " ", " ", " ", " "};
	if(header_selected < header_element_count) arrow[header_selected] = asc ? "▲" : "▼";
        
	mvwprintw(ui_header, 0, 0, proc_array_tab_header[0]);
        mvwprintw(ui_header, 1, 0, proc_array_tab_header[1], arrow[0],arrow[1],arrow[2],arrow[3],arrow[4],arrow[5],arrow[6]);
        mvwprintw(ui_header, 2, 0, proc_array_tab_header[2]);

	wrefresh(ui_header);
}

const char* state_to_str(char s)
{
	switch(s) {
		case 'R': return "RUNNING";
		case 'S': return "SLEEPING";
		case 'D': return "DISK_SLEEP";	
		case 'Z': return "ZOMBIE";
		case 'T': return "STOPPED";
		case 't': return "TRACED";
		case 'W': return "WAKING";
		case 'K': return "WAKEKILL";
		case 'P': return "PARKED";
		case 'X': return "DEAD";	
		case 'x': return "DEAD";	
		case 'I': return "IDLE";
		default: return "UNKNOW";
	}
}

const char* format_mem(const unsigned long long rss_bytes, double *value)
{
	const char *units[] = {"  B", "KiB", "MiB", "GiB", "TiB"};
	*value = (double)rss_bytes;
	int i = 0;

	while (*value >= 1024.0 && i < 4) {
		*value /= 1024.0;
		i++;
	}

	return units[i];
}

void ticks_to_ddhhmmssms(unsigned long ticks, char *buf, size_t bufsize) {
    long ticks_per_sec = sysconf(_SC_CLK_TCK);
    unsigned long total_s = ticks / ticks_per_sec;

    unsigned long s = total_s % 60;
    unsigned long total_m = total_s / 60;
    unsigned long m = total_m % 60;
    unsigned long h = total_m / 60;

    snprintf(buf, bufsize, "%02lu:%02lu:%02lu", h, m, s);
}

void ui_show_proc(const processus_array_t *array,  const size_t selected) {
	/*TEMP*/ werase(ui_pad);

	if(array && array) {
		for(size_t i = 0; i < array->size; ++i)
		{
			double ram;
			const char* unit = format_mem(array->data[i].ram, &ram);
			char buf[32];
			
			ticks_to_ddhhmmssms(array->data[i].start_time, buf, 32);

			if(i == selected) wattron(ui_pad, A_REVERSE);
			mvwprintw(ui_pad, i, 0, separator, 
					array->data[i].pid, 
					array->data[i].user, 
					array->data[i].name, 
					state_to_str(array->data[i].state),
					ram, unit, 
					100.00 //cpu_percent(array, array, )
					, buf
				 );
			if(i == selected) wattroff(ui_pad, A_REVERSE);
		}
	}
}

void update(const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	if(ui_scroll_y > (int)size - terminal_height) {
		ui_scroll_y = abs((int)size - terminal_height) + 4;
	}

	pnoutrefresh(ui_pad, ui_scroll_y, ui_scroll_x, ui_header_lines, 0, terminal_height - ui_footer_lines - 1, terminal_width - 1);

	pnoutrefresh(ui_header, 0, ui_scroll_x, 0, 0, ui_header_lines - 1, terminal_width-1);

	pnoutrefresh(ui_footer, 0, ui_scroll_x, terminal_height-ui_footer_lines, 0, terminal_height-1, terminal_width-1);

	doupdate();
}

void constrain_strict(int *value, const int min, const int max) {
	*value = (*value < min) ? min : (*value > max) ? max : *value;
}

void ui_scroll(const int dx, const int, const size_t selected)
{
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	ui_scroll_x += dx;
	constrain_strict(&ui_scroll_x, 0, ui_pad_columns - terminal_width);

	//ui_scroll_y += dy;
	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;
	const int hi = ui_scroll_y;
	const int lo = hi + view_height - 1;

	if((int)selected < hi) ui_scroll_y = selected;
	else if((int)selected > lo) ui_scroll_y = selected - view_height + 1;

	constrain_strict(&ui_scroll_y, 0,  ui_pad_lines - view_height);

}
