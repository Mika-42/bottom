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
constexpr size_t header_element_count = 7;

static WINDOW *ui_pad = nullptr;
static WINDOW *ui_footer = nullptr;
static WINDOW *ui_header = nullptr;

static int ui_pad_lines = 8000;
static int ui_pad_columns = 134;

constexpr int ui_footer_lines = 3;
constexpr int ui_header_lines = 3;

static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

const char *proc_array_function_command[] = {
	"┣━━━━━━━━━━━┳┻━━━━━━━━━━━━━┳━━━━━━━━━━━━┻━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━┻━━┳━━━━━━━━━┻━━━━━━┳━━━━━┻━━━━━┳━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] help ┃ [F2] page <- ┃ [F3] page -> ┃ [F4] search ┃ [F5] pause/continue ┃ [F6] terminate ┃ [F7] kill ┃ [F8] reload ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_search_bar[] = {
	"┣━━━━━━━━━━━━┻┳━━━━━━━━━━━━┳━━━━━━━━━━━┳┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] cancel ┃ [F2] Prev  ┃ [F3] Next ┃ Search:                                                                         ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_help_header[] = {
        "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓",
	"┃                                                                                                                                    ┃",
	"┃                                                                                                                                    ┃",
};
const char *proc_array_help[] = {
	"┃ ---------------------- G L O B A L ----------------------                                                                          ┃",
	"┃ [→]     scroll to right                                                                                                            ┃",
	"┃ [←]     scroll to left                                                                                                             ┃",	
	"┃ [↑]     scroll to top                                                                                                              ┃",
	"┃ [↓]     scroll to bottom                                                                                                           ┃",
	"┃                                                                                                                                    ┃",
	"┃ [tab]   select sorting field                                                                                                       ┃",
	"┃ [enter] select ascendant/descendent sort                                                                                           ┃",
	"┃                                                                                                                                    ┃",
	"┃ [F9]    exit from the program                                                                                                      ┃",
	"┃                                                                                                                                    ┃",
	"┃ ----------------- N O R M A L ~ M O D E -----------------                                                                          ┃",
	"┃ [F1]    show the help page                                                                                                         ┃",
	"┃ [F2]    switch to the previous machine processus page                                                                              ┃",
	"┃ [F3]    switch to the next machine processus page                                                                                  ┃",
	"┃ [F4]    switch to the search mode                                                                                                  ┃",
	"┃ [F5]    pause/continue the selected processus                                                                                      ┃",
	"┃ [F6]    terminate the selected processus                                                                                           ┃",
	"┃ [F7]    kill the selected processus                                                                                                ┃",
	"┃ [F8]    reload the selected processus                                                                                              ┃",
	"┃                                                                                                                                    ┃",
	"┃ ----------------- S E A R C H ~ M O D E -----------------                                                                          ┃",
	"┃ [F1]    exit from search mode (go back to normal mode)                                                                             ┃",
	"┃ [F2]    go to the previous occurent that match                                                                                     ┃",
	"┃ [F3]    go to the next occurent that match                                                                                         ┃",
	"┃                                                                                                                                    ┃",
};

const char *proc_array_help_footer[] = {
	"┣━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━┫",
	"┃ [F1] cancel ┃                                                                                                          ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_tab_header[] = {
        "┏━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓",
        "┃ PID      %s ┃ User                   %s ┃ Name                           %s ┃ State    %s ┃ RAM      %s ┃ CPU   %s ┃ Time (hh:mm:ss)   %s ┃",
        "┣━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━┫",
};

const char* separator = "┃ %-10d ┃ %-24.23s ┃ %-32.31s ┃ %-10s ┃ %-6.1f %s ┃ %-6.1f%% ┃  %-18s ┃";

void constrain_strict(int *value, const int min, const int max) {
	*value = (*value < min) ? min : (*value > max) ? max : *value;
}

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

void ui_show_footer(const char **array)
{	
	box(ui_footer, 0, 0);
	
	mvwprintw(ui_footer, 0, 0, array[0]);
	mvwprintw(ui_footer, 1, 0, array[1]);
	mvwprintw(ui_footer, 2, 0, array[2]);
}

void ui_show_header(const size_t header_selected, const bool asc)
{
	box(ui_header, 0, 0);

	char* arrow[header_element_count] = {" ", " ", " ", " ", " ", " ", " "};
	if(header_selected < header_element_count) arrow[header_selected] = asc ? "▲" : "▼";
        
	mvwprintw(ui_header, 0, 0, proc_array_tab_header[0]);
        mvwprintw(ui_header, 1, 0, proc_array_tab_header[1], arrow[0],arrow[1],arrow[2],arrow[3],arrow[4],arrow[5],arrow[6]);
        mvwprintw(ui_header, 2, 0, proc_array_tab_header[2]);

}

void ui_show_proc(const processus_array_t *array,  user_selection_t *s) {
	werase(ui_pad);

	if(!array) return;
	size_t filter_index = 0;
	for(size_t i = 0; i < array->size; ++i)
	{
		double ram;
		const char* unit = format_ram(array->data[i].ram, &ram);

		char buf[32];
		format_time(array->data[i].start_time, buf, 32);

		if(i == s->selected) wattron(ui_pad, A_REVERSE);

		if(s->search_mode) {
			if(strncmp(array->data[i].name, s->input, s->input_length) == 0) {
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
		if(i == s->selected) wattroff(ui_pad, A_REVERSE);	
	}

	if(s->search_mode) {
		for(;filter_index < (size_t)getmaxy(stdscr); ++filter_index) {
			mvwprintw(ui_pad, filter_index, 0, "┃ %-10s ┃ %-24s ┃ %-32s ┃ %-10s ┃ %-10s ┃ %-7s ┃  %-18s ┃", "", "", "", "", "", "", "");
		}
	}
}

void ui_update(const size_t size) {
	int terminal_width = 0;
	int terminal_height = 0;

	getmaxyx(stdscr, terminal_height, terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;

	constrain_strict(&ui_scroll_y, 0, (int)size - view_height);

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
	constrain_strict(&ui_scroll_x, 0, ui_pad_columns - terminal_width);

	const int view_height = terminal_height - ui_header_lines - ui_footer_lines;
	const int hi = ui_scroll_y;
	const int lo = hi + view_height - 1;

	if((int)selected < hi) ui_scroll_y = selected;
	else if((int)selected > lo) ui_scroll_y = selected - view_height + 1;

	constrain_strict(&ui_scroll_y, 0,  ui_pad_lines - view_height);

}

void search_mode_event_dispatcher(const int ch, user_selection_t *s) {
	if(ch ==KEY_F(1)) {
		s->search_mode = false; 
	}
	else if(ch ==KEY_F(2)) return; //TODO
	else if(ch ==KEY_F(3)) return; //TODO
	else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
		if (s->input_length > 0) s->input[--s->input_length] = '\0';
	}
	else if (ch >= 32 && ch <= 126) {
		if (s->input_length < 255)
			s->input[s->input_length++] = ch;
		s->input[s->input_length] = '\0';
	}
	mvwprintw(ui_footer, 1, 49, "%-71.71s", s->input);

}

void show_help_page() {

	werase(ui_pad);	

	box(ui_header, 0, 0);

	mvwprintw(ui_header, 0, 0, proc_array_help_header[0]);
	mvwprintw(ui_header, 1, 0, proc_array_help_header[1]);
	mvwprintw(ui_header, 2, 0, proc_array_help_header[2]);

	wrefresh(ui_header);

	constexpr size_t size = sizeof(proc_array_help) / sizeof(proc_array_help[0]);
	size_t i = 0;
	for(; i < size; ++i)
	{
		mvwprintw(ui_pad, i, 0, proc_array_help[i]);
	}

	for(;i < (size_t)getmaxy(stdscr); ++i) {
		mvwprintw(ui_pad, i, 0, proc_array_help[size - 1]);
	}
}

typedef int (*processus_callback_t)(processus_t *);

processus_callback_t normal_mode_event_dispatcher(const processus_array_t *array[], const int ch, user_selection_t *s) {
	switch(ch) {
		case KEY_F(1): s->help = true; break;	
		case KEY_F(2): if(s->machine_selected != 0) --s->machine_selected; break;
		case KEY_F(3): if(s->machine_selected < s->max_machine - 1) ++s->machine_selected; break;
		case KEY_F(4): s->search_mode = true; break;

		case KEY_F(5): return array[s->machine_selected]->data[s->selected].state != 'T' ? stop_process : cont_process;
		case KEY_F(6): return term_process;
		case KEY_F(7): return kill_process;
		case KEY_F(8): return restart_process;
	}

	return nullptr;
}

int global_event_dispatcher(const int ch, const processus_array_t *array, user_selection_t *s) {

	if(ch == KEY_LEFT) return -1;
	if(ch == KEY_RIGHT) return 1;

	if(!s->help) {
		switch(ch) {
			case '\t':
				if(!s->search_mode) {
					s->header_selected = (s->header_selected + 1) % header_element_count;
				}
				break;

			case '\n': 
				if(!s->search_mode) {
					s->asc = !s->asc; 
				} 
				break;

			case KEY_UP: 
				if(s->selected != 0) {
					--s->selected; 
				} 
				break;
			case KEY_DOWN: 
				if(s->selected < array->size - 1) {
					++s->selected; 
				}
				break;
		}
	}
	return 0;
}

error_code_t ui_main(const processus_array_t array[], user_selection_t *user_selection) {

	ui_init();

	for(;;) {

		auto machine = &array[user_selection->machine_selected];
		auto proc = &machine->data[user_selection->selected];

		const int ch = getch();

		if(user_selection->help) { } else {	
		}

		if(ch == KEY_F(9)) {
			endwin();
			return SUCCESS;
		}

		if(user_selection->help) {
			if(ch == KEY_F(1)) user_selection->help = false;

			show_help_page();
			ui_show_footer(proc_array_help_footer);
		} else {
			if(user_selection->search_mode) {
				ui_show_footer(proc_array_search_bar);
				search_mode_event_dispatcher(ch, user_selection);
			} else {
				auto callback = normal_mode_event_dispatcher(&machine, ch, user_selection);
				if(callback) callback(proc); 
				ui_show_footer(proc_array_function_command);
			}

			ui_show_header(user_selection->header_selected, user_selection->asc);
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
