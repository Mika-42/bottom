#include "ui.h"

int main()
{
	ui_init();

	for(;;)
	{
		//clear();
		ui_scroll(0,0);
		ui_show_fn_cmd();

		switch(getch())
		{
			case '\n': endwin(); return 0;
			case KEY_LEFT: ui_scroll(-2, 0); break;
			case KEY_RIGHT: ui_scroll(2, 0); break;
		}
	}


	return 0;
}
