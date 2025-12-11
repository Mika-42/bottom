#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>

#include "ui_constant.h"
#include "ui_index_array.h"

typedef struct user_selection_t {
	size_t		selected;
	size_t		machine_selected;
	size_t		header_selected;
	bool		asc;
	bool		search_mode;
	size_t		max_machine;
	bool		help;
	char		input[255];
	size_t		input_length;
	index_array_t	indices;
} user_selection_t;

typedef struct ui_t {
	WINDOW *pad;
	WINDOW *footer;
	WINDOW *header;
} ui_t;

void ui_init();

void constrain_strict(int *value, const int min, const int max);

void ui_update(const size_t size);
void ui_scroll(const int dx, const size_t selected);
error_code_t ui_main(const processus_array_t array[], user_selection_t *user_selection);

#endif //UI_H
