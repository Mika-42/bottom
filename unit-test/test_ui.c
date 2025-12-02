#include "ui.h"

int main()
{
	ui_init();
	
	list_t l = nullptr;
	proc_update_list("/proc", &l);

	for(;;)
	{
		ui_show_proc(l);

		switch(getch())
		{
			case '\n': endwin(); return 0;
			case KEY_LEFT: ui_scroll(-2, 0); break;
			case KEY_RIGHT: ui_scroll(2, 0); break;
			case KEY_UP: ui_scroll(0, -2); break;
			case KEY_DOWN: ui_scroll(0, 2); break;
		}
	}


	return 0;
}
