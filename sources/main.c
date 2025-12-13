#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <time.h>
#include "ui.h"
#include "processus_sort.h"
#include "ui_event_dispatcher.h"
#include "command_option.h"

/*Notes
 *	array[0] is ALWAYS the local machine !
 */

atomic_bool running = true;

constexpr size_t one_sec = 1'000'000'000; //nanosec

constexpr struct timespec proc_thread_time_interval = {
		.tv_sec = 0,
		.tv_nsec = one_sec / 2	
};

constexpr struct timespec ui_thread_time_interval = {
		.tv_sec = 0,
		.tv_nsec = one_sec / 40
};

typedef struct double_buffer_t {
	processus_array_t	buffer[2];
	atomic_int		active;
} double_buffer_t;

pthread_t proc_thread;
pthread_t ui_thread;

double_buffer_t *array = nullptr;

user_selection_t s = {
	.selected = 0,
	.machine_selected = 0,
	.header_selected = 0, 
	.asc = true,
	.search_mode = false,
	.max_machine = 2,
	.help = false,
	.input = {0},
	.input_length = 0,
	.indices = {
		.data = nullptr,
		.size = 0,
		.capacity = 0,
	}
};

void *proc_task(void *arg) {
	double_buffer_t *db = arg;

	while (atomic_load_explicit(&running, memory_order_acquire)) {

		pthread_mutex_lock(&s.lock);
		int header = s.header_selected;
		bool asc = s.asc;
		pthread_mutex_unlock(&s.lock);			

		const int index = 1 - atomic_load_explicit(&db->active, memory_order_acquire);
		processus_array_t *proc_list = &db->buffer[index];

		if(proc_array_update(proc_list) != SUCCESS) {
			atomic_store_explicit(&running, false, memory_order_release);
			break;
		}

		proc_compare_t cmp = nullptr;

		switch (header) {
			case 0: cmp = asc ? pid_asc : pid_dsc; break;
			case 1: cmp = asc ? user_asc : user_dsc; break;
			case 2: cmp = asc ? name_asc : name_dsc; break;
			case 3: cmp = asc ? state_asc : state_dsc; break;
			case 4: cmp = asc ? ram_asc : ram_dsc; break;
			case 6:	cmp = asc ? time_asc : time_dsc; break;
		}

		if(cmp) proc_array_sort(proc_list, cmp);

		atomic_store_explicit(&db->active, index, memory_order_release);

		nanosleep(&proc_thread_time_interval, nullptr);

	}

	return nullptr;
}

void *ui_task(void *arg) {

	ui_t ui = {0};
	ui_init(&ui);

	size_t select = 0;

	while (atomic_load_explicit(&running, memory_order_acquire)) {

		const int ch = getch();

		pthread_mutex_lock(&s.lock);

		if (ch == KEY_F(9)) {
			pthread_mutex_unlock(&s.lock);
			atomic_store_explicit(&running, false, memory_order_release);
			break;
		}

		double_buffer_t *machines = arg;	
		double_buffer_t *db = &machines[s.machine_selected];
		int index = atomic_load_explicit(&db->active, memory_order_acquire);
		processus_array_t *proc_list = &db->buffer[index];

		if(s.help) {	
			ui.ui_scroll_y = 0;       
			ui_event_dispatcher_help(ch, &ui, &s);
			select = 0;
		} else if (s.search_mode) {

			ui_event_dispatcher_search(proc_list, ch, &ui, &s);
			select = s.selected;

		} else {
			ui_event_dispatcher_normal(proc_list, ch, &ui, &s);
			select = s.selected;
		}

		pthread_mutex_unlock(&s.lock);

		const int scroll_factor = ui_event_dispatcher_global(ch);
		ui_scroll(&ui, scroll_factor, select);
		ui_update(&ui, proc_list->size);

		nanosleep(&ui_thread_time_interval, nullptr);
	}
	
	ui_deinit(&ui);

	return nullptr;
}

error_code_t alloc_data() {

	if(pthread_mutex_init(&s.lock, nullptr) != 0) {
		return THREAD_FAILED;
	}

	array = calloc(s.max_machine, sizeof(*array));
	if (!array) return MEMORY_ALLOCATION_FAILED;

	for(size_t i = 0; i < s.max_machine; ++i) {
		proc_array_init(&array[i].buffer[0]);
		proc_array_init(&array[i].buffer[1]);

		atomic_store_explicit(&array[i].active, 0, memory_order_release);
	}

	if(pthread_create(&proc_thread, nullptr, proc_task, &array[0]) != 0) {
		free(array);
		pthread_mutex_destroy(&s.lock);
		return THREAD_FAILED;
	}

	if(pthread_create(&ui_thread, nullptr, ui_task, array) != 0) {
		atomic_store_explicit(&running, false, memory_order_release);
		pthread_join(proc_thread, nullptr);
		free(array);
		pthread_mutex_destroy(&s.lock);
		return THREAD_FAILED;
	}

	return SUCCESS;
}

void release_data() {
	for (size_t i=0; i<s.max_machine; ++i) {
		proc_array_free(&array[i].buffer[0]);
		proc_array_free(&array[i].buffer[1]);
	}

	free(array);

	pthread_mutex_destroy(&s.lock);
	ui_index_array_free(&s.indices);
}

int main(/*int argc, char *argv[]*/){	
	/*
	   options_prog options = {
	   .help = false,
	   .dry_run = false,
	   .remote_config = NULL,
	   .connexion_type = NULL,
	   .port = 0, 
	   .login = NULL,
	   .remote_server = NULL,
	   .username = NULL,
	   .password = NULL,
	   .all = false
	   };

	   int gerer_options = command_run(argc, argv, &options);

	   if (gerer_options !=0) {
	   fprintf(stderr, "Erreur lors de l'analyse des options. Utilisez -h pour l'aide.\n");
	   return EXIT_FAILURE;
	   }
	   */

	if(alloc_data() != SUCCESS) return EXIT_FAILURE;


	pthread_join(proc_thread, nullptr);
	pthread_join(ui_thread, nullptr);
	
	release_data();
}

