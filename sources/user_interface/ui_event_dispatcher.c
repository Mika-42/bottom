#include "ui_event_dispatcher.h"
#include "signal_process.h"

processus_callback_t ui_event_dispatcher_normal(const processus_array_t *array[], const int ch, user_selection_t *s) {
	switch(ch) {
		case KEY_F(1): s->help = true; break;
		case KEY_F(2):
			       {
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

void ui_event_dispatcher_help(const int ch, user_selection_t *s) {
	if(ch == KEY_F(1)) {
		s->help = false;
	}
}
