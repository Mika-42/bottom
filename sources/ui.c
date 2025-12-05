#include "ui.h"

#include <locale.h>

static WINDOW *ui_pad = nullptr;

static int ui_pad_lines = 200;
static int ui_pad_columns = 134;

static int ui_scroll_x = 0;
static int ui_scroll_y = 0;

void ui_init()
{
	setlocale(LC_ALL,"");
	initscr();
	start_color();
        use_default_colors();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        flushinp();
        curs_set(0);
	nodelay(stdscr, TRUE);
	ui_pad = newpad(ui_pad_lines, ui_pad_columns);
}

void ui_show_fn_cmd()
{
	/*TEMP*/ werase(ui_pad);

	// mvwprintw(ui_pad, 0, 0, "┏━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━┓");
	// mvwprintw(ui_pad, 1, 0, "┃ [F1] help ┃ [F2] page -> ┃ [F3] page <- ┃ [F4] search ┃ [F5] pause/continue ┃ [F6] terminate ┃ [F7] kill ┃ [F8] reload ┃ [F9] Exit ┃");
	// mvwprintw(ui_pad, 2, 0, "┗━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┻━━━━━━━━━━━┛");

//	/*TEMP*/ ui_show_proc();

	int terminal_width = 0;
        int terminal_height = 0;

        getmaxyx(stdscr, terminal_height, terminal_width);
	
	pnoutrefresh(ui_pad, ui_scroll_y, ui_scroll_x, 0, 0, terminal_height - 1, terminal_width - 1);
    	doupdate();
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
		default:		return "UNKNOW";
	}
}

void ui_show_proc(list_t l)
{
	/*TEMP*/ werase(ui_pad);

	size_t i = 0;
        mvwprintw(ui_pad, i++, 0, "┏━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━┓");
        mvwprintw(ui_pad, i++, 0, "┃ PID        ┃ User                             ┃ Name                             ┃ State      ┃ RAM/RSS    ┃ CPU time   ┃ Time     ┃");
        mvwprintw(ui_pad, i++, 0, "┣━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━━━╋━━━━━━━━━━┫");
	for(; l != nullptr; l = l->next)
	{
        	mvwprintw(ui_pad, i++, 0, "┃ %-10d ┃ %-32s ┃ %-32s ┃ %-10s ┃ %-10ld ┃            ┃          ┃", 
				l->data.pid, 
				l->data.user, 
				l->data.name,
				state_to_str(l->data.state),
				l->data.ram_rss
				);
	}

	mvwprintw(ui_pad, i++, 0, "┗━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━━━┻━━━━━━━━━━┛");
        
	
	int terminal_width = 0;
        int terminal_height = 0;

        getmaxyx(stdscr, terminal_height, terminal_width);

        pnoutrefresh(ui_pad, ui_scroll_y, ui_scroll_x, 0, 0, terminal_height - 1, terminal_width - 1);
        doupdate();
}

void ui_scroll(const int dx, const int dy)
{
	int terminal_width = 0;
	int terminal_height = 0;
	
	getmaxyx(stdscr, terminal_height, terminal_width);
		
	ui_scroll_x += dx;
	ui_scroll_y += dy;

	if(ui_scroll_y < 0) ui_scroll_y = 0;
	
	if(ui_scroll_y > ui_pad_lines - terminal_height) 
	{
		ui_scroll_y = ui_pad_lines - terminal_height;
	}


	if(ui_scroll_x < 0) ui_scroll_x = 0;
  
	if(ui_scroll_x > ui_pad_columns - terminal_width) 
	{
		ui_scroll_x = ui_pad_columns - terminal_width;
	}
	
}
