#include "ui.h"

int main()
{
	ui_init();
	
	processus_array_t l = {0};

	
	//for(;;);

	int scroll_factor = 10;
	for(;;)
	{

		proc_array_update("/proc", &l);
		ui_show_proc(&l);

		switch(getch())
		{
			case '\n': endwin(); return 0;
			case KEY_LEFT: ui_scroll(-scroll_factor, 0, &l); break;
			case KEY_RIGHT: ui_scroll(scroll_factor, 0, &l); break;
			case KEY_UP: ui_scroll(0, -scroll_factor, &l); break;
			case KEY_DOWN: ui_scroll(0, scroll_factor, &l); break;
		}
	}

	proc_array_free(&l);

	return 0;
}
