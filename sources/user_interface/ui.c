#include "ui.h"

#include <stdlib.h>
#include <locale.h>
#include <limits.h>

static WINDOW *ui_pad = nullptr;
static WINDOW *ui_footer = nullptr;
static WINDOW *ui_header = nullptr;

static int ui_pad_lines = 5000;
static int ui_pad_columns = 134;

static int ui_footer_lines = 3;
static int ui_header_lines = 3;

static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

const char *proc_array_function_command[] = {
	"┣━━━━━━━━━━━┳┻━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━┻━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┳━━━━┻━━━━━━━━━━━┳┻━━━━━━━━━━┳━┻━━━━━━━━━━━┳┻━━━━━━━━━━┫",
	"┃ [F1] help ┃ [F2] page -> ┃ [F3] page <- ┃ [F4] search ┃ [F5] pause/continue ┃ [F6] terminate ┃ [F7] kill ┃ [F8] reload ┃ [F9] Exit ┃",
	"┗━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━┛",
};

const char *proc_array_tab_header[] = {
        "┏━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━┓",
        "┃ PID        ┃ User                             ┃ Name                             ┃ State      ┃ RAM/RSS    ┃ CPU usage  ┃ Time     ┃",
        "┣━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━┫",
};

const char* separator = "┃ %-10d ┃ %-32s ┃ %-32s ┃ %-10s ┃ %-10ld ┃            ┃          ┃";

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

void ui_show_tab_header()
{
	werase(ui_header);
	box(ui_header, 0, 0);

        mvwprintw(ui_header, 0, 0, proc_array_tab_header[0]);
        mvwprintw(ui_header, 1, 0, proc_array_tab_header[1]);
        mvwprintw(ui_header, 2, 0, proc_array_tab_header[2]);

	wrefresh(ui_header);
}

const char* state_to_str(proc_state_t s)
{
	switch(s) {
		case RUNNING:		return "RUNNING";
		case SLEEPING:		return "SLEEPING";
		case DISK_SLEEP:	return "DISK_SLEEP";	
		case ZOMBIE:		return "ZOMBIE";
		case STOPPED:		return "STOPPED";
		case TRACED:		return "TRACED";
		case WAKING:		return "WAKING";
		case WAKEKILL:		return "WAKEKILL";
		case PARKED:		return "PARKED";
		case DEAD:		return "DEAD";	
		case IDLE:		return "IDLE";
		default:		return "UNKNOW";
	}
}

void ui_show_proc(const processus_array_t *array, const size_t selected)
{
	/*TEMP*/ werase(ui_pad);
		
	if(array) {
		for(size_t i = 0; i < array->size; ++i)
		{
			if(i == selected) wattron(ui_pad, A_REVERSE);
       	 		mvwprintw(ui_pad, i, 0, separator, 
					array->data[i].pid, array->data[i].user, 
					array->data[i].name, state_to_str(array->data[i].state),
					array->data[i].ram_rss
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
