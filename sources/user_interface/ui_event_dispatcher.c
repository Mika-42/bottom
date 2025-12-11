#include "ui_event_dispatcher.h"
#include "ui_constant.h"
#include "ui_page.h"
#include "ui_utils.h"
#include "signal_process.h"

#include <string.h>

processus_callback_t ui_event_dispatcher_normal(const processus_array_t *array[], const int ch, ui_t *ui, user_selection_t *s) {
	switch(ch) {
		case KEY_F(1): s->help = true; break;
		case KEY_F(2): {
				       if(s->machine_selected != 0) {
					       --s->machine_selected;
				       }
				       break;
			       }
		case KEY_F(3): {
				       if(s->machine_selected < s->max_machine - 1) {
					       ++s->machine_selected;
				       }
				       break;
			       }
		case KEY_F(4): s->search_mode = true; break;
			       
		case KEY_F(5): {
				       auto machine = array[s->machine_selected];
				       auto proc = &(machine->data[s->selected]);
				       return proc->state != 'T' ? stop_process : cont_process;
			       }
		case KEY_F(6): return term_process;
		case KEY_F(7): return kill_process;
		case KEY_F(8): return restart_process;
		case KEY_UP:
			   if (s->selected != 0) {
				   --s->selected;
			   }
			   break;
		case KEY_DOWN:
			   if (s->selected < array[s->machine_selected]->size - 1) {
				   ++s->selected;
			   }
			   break;
	}
	
	//todo constrain selected in range [0, index_array->size[
	ui_event_dispatcher_sort(ch, s);
	ui_show_array(ui->footer, proc_array_function_command);
	ui_show_header(s->header_selected, ui, s->asc);

	return nullptr;
}

void ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s) {
	
	s->selected = 0;

	if(ch == KEY_F(1)) {
		s->help = false;
	}

	show_help_page(ui);
}

void ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s) {
	
	ui_utils_clamp_size_t(&s->selected, 0, s->indices.size > 0 ? s->indices.size - 1 : 0);
	
	if (ch == KEY_F(1)) {
		s->search_mode = false;
		memset(s->input, 0, sizeof(s->input));
		if(s->indices.size > 0) s->selected = s->indices.data[s->selected];
		return;
	}
	
	else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
		if (s->input_length > 0) s->input[--s->input_length] = '\0';
	}

	else if (ch >= 32 && ch <= 126) {

		if (s->input_length < 255) {
			s->input[s->input_length++] = ch;
		}

		s->input[s->input_length] = '\0';
	}
	else if(ch == KEY_UP && s->selected != 0) {
                                   --s->selected;
	}
	else if(ch == KEY_DOWN && s->selected < s->indices.size - 1) {
                                   ++s->selected;
	}
		
	ui_event_dispatcher_sort(ch, s);
	ui_show_array(ui->footer, proc_array_search_bar);
	mvwprintw(ui->footer, 1, 24, "%-96.96s", s->input);

	ui_show_header(s->header_selected, ui, s->asc);
}

void ui_event_dispatcher_sort(const int ch, user_selection_t *s) {
	if(ch == '\t') { 
		s->header_selected = (s->header_selected + 1) % header_element_count;
	} else if (ch == KEY_F(12)) {
	       	s->asc = !s->asc;
	}
}

int ui_event_dispatcher_global(const int ch) {

	if (ch == KEY_LEFT) return -1;
	if (ch == KEY_RIGHT) return 1;

	return 0;
}
