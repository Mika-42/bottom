#include "processus_signal.h"
#include "processus_sort.h"
#include "thread.h"
#include "ui_event_dispatcher.h"
#include "ui_page.h"

/* Notes:
 *	array[0] is ALWAYS the local machine !
 */

// pthread_t proc_thread;
// pthread_t ui_thread;
static const proc_compare_t sort_func[2][header_element_count] = {
	{pid_dsc, user_dsc, name_dsc, state_dsc, ram_dsc, cpu_dsc, time_dsc},
	{pid_asc, user_asc, name_asc, state_asc, ram_asc, cpu_asc, time_asc}};

void *ssh_task(void *arg) {

	if (!arg) {
		return nullptr;
	}

	thread_args_t *args = arg;
	user_selection_t *s = &args->selection;

	// 1 if SSH + Local
	// 0 if SSH only
	const size_t start_index = args->exec_local;

	while (atomic_load_explicit(&args->running, memory_order_acquire)) {
		pthread_mutex_lock(&s->lock);
		const int header = s->header_selected;
		const sort_type_t sort = s->sort;
		const proc_event_t evt = s->event;
		const size_t curr = s->selected;
		const size_t machine_index = s->machine_selected;
		pthread_mutex_unlock(&s->lock);

		if (args->exec_local && machine_index == 0) {
			nanosleep(&proc_thread_time_interval, nullptr);
			continue;
		}

		double_buffer_t *db = &args->array[machine_index];

		const int index =
			1 - atomic_load_explicit(&db->active, memory_order_acquire);
		processus_array_t *proc_list = &db->buffer[index];
		auto curr_el = &proc_list->data[curr];
		auto curr_session = args->sessions.data[machine_index + start_index];

		switch (evt) {
			case PAUSE_CONTINUE:
				(curr_el->state == 'T') ? ssh_cont_processus(curr_session, curr_el->pid) : ssh_stop_processus(curr_session, curr_el->pid);

				break;
			case TERMINATE:
				ssh_term_processus(curr_session, curr_el->pid);
				break;
			case KILL:
				ssh_kill_processus(curr_session, curr_el->pid);
				break;
			case RELOAD:
				ssh_restart_processus(curr_session, curr_el->pid);
				break;
			default:
				break;
		}

		if (evt != NOTHING) {
			pthread_mutex_lock(&s->lock);
			s->event = NOTHING;
			pthread_mutex_unlock(&s->lock);
		}

		/**/

		if ((size_t)header < header_element_count) {
			proc_array_sort(proc_list, sort_func[sort][header]);
		}
		nanosleep(&proc_thread_time_interval, nullptr);

		atomic_store_explicit(&db->active, index, memory_order_release);

	}
	return nullptr;
}

void *proc_task(void *arg) {

	if (!arg) {
		return nullptr;
	}

	thread_args_t *args = arg;

	double_buffer_t *db = &args->array[0];
	user_selection_t *s = &args->selection;

	while (atomic_load_explicit(&args->running, memory_order_acquire)) {

		pthread_mutex_lock(&s->lock);
		const int header = s->header_selected;
		const sort_type_t sort = s->sort;
		const proc_event_t evt = s->event;
		const size_t curr = s->selected;
		pthread_mutex_unlock(&s->lock);

		const int index =
			1 - atomic_load_explicit(&db->active, memory_order_acquire);
		processus_array_t *proc_list = &db->buffer[index];
		auto curr_el = &proc_list->data[curr];

		switch (evt) {
			case PAUSE_CONTINUE:
				(curr_el->state == 'T') ? proc_cont(curr_el) : proc_stop(curr_el);
				break;
			case TERMINATE:
				proc_term(curr_el);
				break;
			case KILL:
				proc_kill(curr_el);
				break;
			case RELOAD:
				proc_restart(curr_el);
				break;
			default:
				break;
		}

		if (evt != NOTHING) {
			pthread_mutex_lock(&s->lock);
			s->event = NOTHING;
			pthread_mutex_unlock(&s->lock);
		}

		if (proc_array_update(proc_list) != SUCCESS) {
			atomic_store_explicit(&args->running, false, memory_order_release);
			break;
		}

		if (proc_array_get_cpu(&db->buffer[1 - index], &db->buffer[index]) !=
				SUCCESS) {
			atomic_store_explicit(&args->running, false, memory_order_release);
			break;
		}

		if ((size_t)header < header_element_count) {
			proc_array_sort(proc_list, sort_func[sort][header]);
		}
		nanosleep(&proc_thread_time_interval, nullptr);

		atomic_store_explicit(&db->active, index, memory_order_release);
	}

	return nullptr;
}

void *ui_task(void *arg) {

	if (!arg) {
		return nullptr;
	}

	ui_t ui = {0};
	ui_init(&ui);

	size_t select = 0;

	thread_args_t *args = arg;
	user_selection_t *s = &args->selection;
	double_buffer_t *machines = args->array;
	struct timespec last_update = {0, 0};

	while (atomic_load_explicit(&args->running, memory_order_acquire)) {

		const int ch = getch();

		pthread_mutex_lock(&s->lock);

		if (ch == KEY_F(9)) {
			pthread_mutex_unlock(&s->lock);
			atomic_store_explicit(&args->running, false, memory_order_release);
			break;
		}

		double_buffer_t *db = &machines[s->machine_selected];
		int index = atomic_load_explicit(&db->active, memory_order_acquire);
		processus_array_t *proc_list = &db->buffer[index];

		if (s->mode == HELP) {
			ui.ui_scroll_y = 0;
			ui_event_dispatcher_help(ch, &ui, s);
			select = 0;
		} else {
			if (s->mode == SEARCH) {
				ui_event_dispatcher_search(ch, &ui, s);
			} else {
				ui_event_dispatcher_normal(proc_list, ch, &ui, s);
			}

			// non-bloking delay 200ms
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			if ((now.tv_sec - last_update.tv_sec) * 1000 +
					(now.tv_nsec - last_update.tv_nsec) / 1000000 >=
					250) {
				if (ui_show_proc(proc_list, &ui, s) != SUCCESS) {

					pthread_mutex_unlock(&s->lock);
					atomic_store_explicit(&args->running, false, memory_order_release);
					break;
				}
				last_update = now;
			}

			ui_select(&ui, s);

			select = s->selected;
		}
		const int scroll_factor = ui_event_dispatcher_global(ch);

		ui_scroll(&ui, scroll_factor, select);

		ui_update(&ui, proc_list->size);

		pthread_mutex_unlock(&s->lock);

		nanosleep(&ui_thread_time_interval, nullptr);
	}

	ui_deinit(&ui);

	return nullptr;
}

void release_data(thread_args_t *args) {

	if (!args) {
		return;
	}
	if (args->array) {
		for (size_t i=0; i< args->selection.max_machine; ++i) {
			proc_array_free(&args->array[i].buffer[0]);
			proc_array_free(&args->array[i].buffer[1]);
		}

		free(args->array);
	}

	pthread_mutex_destroy(&args->selection.lock);
	ui_index_array_free(&args->selection.indices);
}
