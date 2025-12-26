#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>
#include "ui_constant.h"
#include "ui_index_array.h"
#include <pthread.h>

typedef struct user_selection_t {
	size_t		selected;
	size_t		machine_selected;
	size_t		header_selected;
	
	//----
	sort_type_t		sort;	// proc_task read-only
	user_mode_t		mode;	// proc_task read-only
	proc_event_t	event;	// proc_task read-write
	//----
	size_t		max_machine;
	char		input[256];
	size_t		input_length;
	index_array_t	indices;
	pthread_mutex_t lock;
} user_selection_t;

typedef struct ui_t {
	WINDOW *pad;
	WINDOW *footer;
	WINDOW *header;

	int ui_scroll_x;
	int ui_scroll_y;
} ui_t;

void ui_init(ui_t *ui);
void ui_deinit(ui_t *ui);
void ui_update(ui_t *ui, const size_t size);
void ui_scroll(ui_t *ui, const int dx, const size_t selected);
void ui_select(ui_t *ui, user_selection_t *s);

#endif // UI_H
