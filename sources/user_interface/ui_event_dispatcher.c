#include "ui_page.h"
#include "ui_event_dispatcher.h"
#include "signal_process.h"

processus_callback_t ui_event_dispatcher_normal(const processus_array_t *array[], const int ch, user_selection_t *s) {
	switch(ch) {
		case KEY_F(1): s->help = true; break;
		case KEY_F(2): {
				       if(s->machine_selected != 0) {
					       --s->machine_selected;
				       }
				       break;
			       }
		case KEY_F(3):
			       {
				       if(s->machine_selected < s->max_machine - 1) {
					       ++s->machine_selected;
				       }
				       break;
			       }
		case KEY_F(4): s->search_mode = true; break;

		case KEY_F(5):
			       {
				       auto machine = array[s->machine_selected];
				       auto proc = &(machine->data[s->selected]);
				       return proc->state != 'T' ? stop_process : cont_process;
			       }
		case KEY_F(6): return term_process;
		case KEY_F(7): return kill_process;
		case KEY_F(8): return restart_process;
	}

	return nullptr;
}

void ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s) {
	
	if(ch == KEY_F(1)) {
		s->help = false;
	}

	show_help_page(ui->pad, ui->header, ui->footer);
}

void ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s) {
	if (ch == KEY_F(1)) {
		s->search_mode = false;
		return;
	}

	if (ch == KEY_F(2)) return; //TODO
	if (ch == KEY_F(3)) return; //TODO
	
	else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
		if (s->input_length > 0) s->input[--s->input_length] = '\0';
	}

	else if (ch >= 32 && ch <= 126) {
		
		if (s->input_length < 255) {
			s->input[s->input_length++] = ch;
		}

		s->input[s->input_length] = '\0';
	}

	ui_show_array(ui->footer, proc_array_search_bar);
	mvwprintw(ui->footer, 1, 49, "%-71.71s", s->input);
}
