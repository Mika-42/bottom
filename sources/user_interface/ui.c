#include "ui.h"
#include "format.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include "signal_process.h"

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

const char *proc_array_help[] = {
	"┃ [→] scroll to right                                                                                                                ┃",
	"┃ [←] scroll to left                                                                                                                 ┃",	
	"┃ [↑] scroll to top                                                                                                                  ┃",
	"┃ [↓] scroll to bottom                                                                                                               ┃",
	"┃                                                                                                                                    ┃",
	"┃ [tab]   select sorting field                                                                                                       ┃",
	"┃ [enter] select ascendant/descendent sort                                                                                           ┃",
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
	werase(ui_footer);
	box(ui_footer, 0, 0);
	
	mvwprintw(ui_footer, 0, 0, array[0]);
	mvwprintw(ui_footer, 1, 0, array[1]);
	mvwprintw(ui_footer, 2, 0, array[2]);

	wrefresh(ui_footer);
}

void ui_show_header(const size_t header_selected, const bool asc)
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
void ui_show_proc(const processus_array_t *array,  const size_t selected) {
	werase(ui_pad);

	if(!array) return;

	for(size_t i = 0; i < array->size; ++i)
	{
		double ram;
		const char* unit = format_ram(array->data[i].ram, &ram);
		
		char buf[32];
		format_time(array->data[i].start_time, buf, 32);

		if(i == selected) wattron(ui_pad, A_REVERSE);
		mvwprintw(ui_pad, i, 0, separator, 
				array->data[i].pid, 
				array->data[i].user, 
				array->data[i].name, 
				format_state(array->data[i].state),
				ram, unit, 
				100.00 //cpu_percent(array, array, )
				, buf
			 );
		if(i == selected) wattroff(ui_pad, A_REVERSE);
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
	switch(ch) {
		case KEY_F(1): s->search_mode = false;
	}
}

typedef int (*processus_callback_t)(processus_t *);

processus_callback_t normal_mode_event_dispatcher(/*const*/ processus_array_t *array[], const int ch, user_selection_t *s) {
	switch(ch) {
		case KEY_F(1): break; //todo	
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

	switch(ch) {
		case '\t':
			s->header_selected = (s->header_selected + 1) % header_element_count; break;
		case '\n': s->asc = !s->asc; break;

		case KEY_LEFT: return -1;
		case KEY_RIGHT: return 1;
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
	return 0;
}

error_code_t ui_main(/*const*/ processus_array_t array[], user_selection_t *user_selection) {

	ui_init();

	for(;;) {

		auto machine = &array[user_selection->machine_selected];
		auto proc = &machine->data[user_selection->selected];

		/*TEMP*/ // proc_array_update(machine);
		
		ui_show_header(user_selection->header_selected, user_selection->asc);
		ui_show_proc(machine, user_selection->selected);
		ui_show_footer(!user_selection->search_mode ? proc_array_function_command : proc_array_search_bar);

		const int ch = getch();

		if(ch == KEY_F(9)) {
			endwin();
			return SUCCESS;
		}

		if(user_selection->search_mode) {
			search_mode_event_dispatcher(ch, user_selection);
		} else {
			auto callback = normal_mode_event_dispatcher(&machine, ch, user_selection);
			if(callback) callback(proc);
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
