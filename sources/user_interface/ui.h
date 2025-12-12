#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>
#include <pthread.h>
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
	pthread_mutex_t lock;
} user_selection_t;

typedef struct ui_t {
	WINDOW *pad;
	WINDOW *footer;
	WINDOW *header;
} ui_t;

void		ui_init(ui_t *ui);
void		ui_update(ui_t* ui, const size_t size);
void		ui_scroll(const int dx, const size_t selected);

#endif //UI_H
