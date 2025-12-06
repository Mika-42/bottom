#include "ui.h"
#include <math.h>

size_t min(const size_t lhs, const size_t rhs)
{
	return (lhs <= rhs) ? lhs : rhs;
}


size_t max(const size_t lhs, const size_t rhs)
{
	return (lhs >= rhs) ? lhs : rhs;
}

int main()
{
	ui_init();
	
	processus_array_t l = {0};

	int scroll_factor = 1;
	size_t selected = 0;

	for(;;)
	{
		
		proc_array_update("/proc", &l);
		ui_show_fn_cmd();
		ui_show_tab_header();
		ui_show_proc(&l, selected);
		
		switch(getch())
		{
			case '\n': endwin(); return 0;
			case KEY_LEFT: ui_scroll(-scroll_factor, 0, selected); break;
			case KEY_RIGHT: ui_scroll(scroll_factor, 0, selected); break;
			case KEY_UP: 
					if(selected > 0) --selected; 	
					ui_scroll(0, -scroll_factor, selected); 
					break;
			case KEY_DOWN: 
			
					selected = min(l.size, ++selected); 
					ui_scroll(0, scroll_factor, selected); 
					break;
		}

		update(l.size);	
	}

	proc_array_free(&l);

	return 0;
}
