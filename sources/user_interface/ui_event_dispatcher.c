#include "ui_event_dispatcher.h"
#include "ui_constant.h"
#include "ui_page.h"
#include "ui_utils.h"
#include "processus_signal.h"

void ui_event_dispatcher_normal(const processus_array_t *array, const int ch, ui_t *ui, user_selection_t *s) {

	if (!array || !ui || !s) {
		return;
	}

	if (array->size != 0 && s->selected>=array->size) {
		s->selected = array->size - 1;
	}

	if (ch == KEY_F(1)) {
		s->mode = HELP;
	} else if (ch == KEY_F(2) && s->machine_selected != 0) {
		--s->machine_selected;
	} else if (ch == KEY_F(3) && s->machine_selected < s->max_machine - 1) {
		++s->machine_selected;
	} else if (ch == KEY_F(4)) { 
		s->mode = SEARCH;
		ui_utils_reset_input_buffer(s);
	} else if (ch == KEY_F(5)) {
		s->event = PAUSE_CONTINUE;
	} else if (ch == KEY_F(6)) {
		s->event = TERMINATE;
	} else if (ch == KEY_F(7)) {
		s->event = KILL;
	} else if (ch == KEY_F(8)) {
		s->event = RELOAD;
	} else if (ch == KEY_UP && s->selected != 0) {
		--s->selected;
	} else if (ch == KEY_DOWN && s->selected < array->size - 1) {
		++s->selected;
	} 
	
	ui_event_dispatcher_sort(ch, s);
	ui_show_array(ui->footer, proc_array_function_command);
	ui_show_header(s->header_selected, ui, s->sort);
}

void ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s) {
	
	if (ui) {
	if (ch == KEY_F(1)) {
		s->mode = NORMAL;
	}

	show_help_page(ui);
	}
}

void ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s) {

	if (!ui || !s) {
		return;
	}

	ui_utils_clamp_size_t(&s->selected, 0, s->indices.size > 0 ? s->indices.size - 1 : 0);

	if (ch == KEY_F(1)) {
		s->mode = NORMAL;
		ui_utils_reset_input_buffer(s);
		if (s->indices.size > 0) {
			s->selected = s->indices.data[s->selected];
		}
		return;
	} else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
		if (s->input_length > 0) {
			s->input[--s->input_length] = '\0';
		}
	} else if (ch >= 32 && ch <= 126) {

		if (s->input_length < 255) {
			s->input[s->input_length++] = ch;
			s->selected = 0;
		}

		s->input[s->input_length] = '\0';
	} else if (ch == KEY_UP && s->selected != 0) {
		--s->selected;
	} else if (ch == KEY_DOWN && s->selected < s->indices.size - 1) {
		++s->selected;
	}

	ui_event_dispatcher_sort(ch, s);
	ui_show_array(ui->footer, proc_array_search_bar);
	mvwprintw(ui->footer, 1, 24, "%-96.96s", s->input);

	ui_show_header(s->header_selected, ui, s->sort);
}

void ui_event_dispatcher_sort(const int ch, user_selection_t *s) {
	
	if (!s) {
		return;
	}

	if (ch == '\t') { 
		s->header_selected = (s->header_selected + 1) % header_element_count;
	} else if (ch == '\n') {
		s->sort = s->sort == ASC ? DSC : ASC;
	}
}

int ui_event_dispatcher_global(const int ch) {

	if (ch == KEY_LEFT) {
		return -1;
	}
	if (ch == KEY_RIGHT) {
		return 1;
	}

	return 0;
}
