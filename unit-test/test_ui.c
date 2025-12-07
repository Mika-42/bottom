#include "ui.h"
#include <math.h>
#include<unistd.h>

constexpr size_t header_element_count = 7;

size_t min(const size_t lhs, const size_t rhs)
{
	return (lhs <= rhs) ? lhs : rhs;
}


size_t max(const size_t lhs, const size_t rhs)
{
	return (lhs >= rhs) ? lhs : rhs;
}

void sort_dispatcher(processus_array_t* array, const size_t header_selected, const bool asc) 
{
	proc_compare_t cmp = nullptr;

	switch(header_selected)
	{
		case 0: cmp = asc ? pid_asc : pid_dsc; break;
		case 1: cmp = asc ? user_asc : user_dsc; break;
		case 2: cmp = asc ? name_asc : name_dsc; break;
		case 3: cmp = asc ? state_asc : state_dsc; break;
		case 4: cmp = asc ? ram_asc : ram_dsc; break;
		//todo 5 
		case 6:	cmp = asc ? time_asc : time_dsc; break;
	}

	proc_array_sort(array, cmp);

}

int main()
{
	ui_init();

	processus_array_t l = {0};

	//---- USER DEFINE
	int scroll_factor = 1;
	//----
	size_t selected = 0;
	size_t header_selected = 0;
	bool asc = true;
	bool search_mode = false;

	for(;;)
	{

		//thread 2
		proc_array_update(&l);
		//		sleep(1);

		sort_dispatcher(&l, header_selected, asc);
		//---

		ui_show_tab_header(header_selected, asc);
		ui_show_proc(&l, selected);

		const int ch = getch();	
	
		if(ch == KEY_F(9)) {
			endwin();
			return 0;
		}

		if(search_mode) {
			ui_show_search_bar();
			switch(ch) {
				case KEY_F(1): search_mode = false; break;	
			}
		} else {
			ui_show_fn_cmd();
			switch(ch) {
				case KEY_F(4): search_mode = true; break;	
				case '\t':
					       header_selected = (header_selected + 1) % header_element_count; break;
				case '\n': asc = !asc; break;

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
		}
		update(l.size);	
	}

	proc_array_free(&l);

	return 0;
}
