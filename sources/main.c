#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "command_option.h"
#include "ui.h"
#include <stdatomic.h>
#include <time.h>
#include "processus_sort.h"
#include "ui_event_dispatcher.h"

atomic_bool running = true;

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



void *proc_task(void*) {

	constexpr struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 1'000'000'000 / 2 // 1/4 seconde
	};

	while (atomic_load_explicit(&running, memory_order_acquire)) {
		
		if (!array) break;
		
		const int index = 1 - atomic_load_explicit(&(array[0].active), memory_order_acquire);
		
		auto proc_list = &(array[0].buffer[index]);


		pthread_mutex_lock(&s.lock);
		// local is always first machine
		if (s.max_machine > 0) {
			if(proc_array_update(proc_list) != SUCCESS) {

				pthread_mutex_unlock(&s.lock);
				atomic_store_explicit(&running, false, memory_order_release);
				break;
			}
		}

		proc_compare_t cmp = nullptr;

		switch (s.header_selected) {
			case 0: cmp = s.asc ? pid_asc : pid_dsc; break;
			case 1: cmp = s.asc ? user_asc : user_dsc; break;
			case 2: cmp = s.asc ? name_asc : name_dsc; break;
			case 3: cmp = s.asc ? state_asc : state_dsc; break;
			case 4: cmp = s.asc ? ram_asc : ram_dsc; break;
			case 6:	cmp = s.asc ? time_asc : time_dsc; break;
		}
	
		proc_array_sort(proc_list, cmp);
	
		atomic_store_explicit(&(array[0].active), index, memory_order_release);

		pthread_mutex_unlock(&s.lock);
		
		nanosleep(&ts, nullptr);

	}

	return nullptr;
}

void *ui_task(void*) {
	ui_t ui = {0};
	ui_init(&ui);

	constexpr struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 1'000'000'000 / 40 // 1/40 seconde
	};

	size_t select = 0;
	
	for (;;) {
		
		const int ch = getch();
		
		pthread_mutex_lock(&s.lock);
		
		int index = atomic_load_explicit(&(array[s.machine_selected].active), memory_order_acquire);
		auto proc_list = &(array[s.machine_selected].buffer[index]);

		if (ch == KEY_F(9)) {
			endwin(); break;
		}

		if(s.help) {	
		//TODO remettre le ui_scroll_y à 0//	ui_scroll_y = 0;       
			ui_event_dispatcher_help(ch, &ui, &s);
			select = 0;
		} else {
			if (s.search_mode) {

				ui_event_dispatcher_search(proc_list, ch, &ui, &s);


			} else {
				ui_event_dispatcher_normal(proc_list, ch, &ui, &s);
			}

			select = s.selected;
		}
		pthread_mutex_unlock(&s.lock);

		const int scroll_factor = ui_event_dispatcher_global(ch);
		ui_scroll(scroll_factor, select);
		ui_update(&ui, proc_list->size);

		nanosleep(&ts, nullptr);
	}

	atomic_store_explicit(&running, false, memory_order_release);
	return nullptr;
}

error_code_t alloc_data() {

	pthread_mutex_init(&s.lock, nullptr);
	
	array = calloc(s.max_machine, sizeof(*array));
	if (!array) return MEMORY_ALLOCATION_FAILED;

	if(pthread_create(&proc_thread, nullptr, proc_task, nullptr) != 0) {
		free(array);
		pthread_mutex_destroy(&s.lock);
        	return THREAD_FAILED;
	}

	if(pthread_create(&ui_thread, nullptr, ui_task, nullptr) != 0) {
		atomic_store_explicit(&running, false, memory_order_release);
		pthread_join(proc_thread, nullptr);
		free(array);
                pthread_mutex_destroy(&s.lock);
                return THREAD_FAILED;
	}

	return SUCCESS;
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

	for (size_t i=0; i<s.max_machine; ++i) {
		proc_array_free(&array[i].buffer[0]);
		proc_array_free(&array[i].buffer[1]);
	}

	free(array);

	ui_index_array_free(&s.indices);
//rember to free ui.pad, ui.footer, ui.header
	pthread_mutex_destroy(&s.lock);
}

