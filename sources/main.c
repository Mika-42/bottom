#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "command_option.h"
#include "ui.h"
#include <stdatomic.h>
#include <time.h>

atomic_bool running = true;
processus_array_t *array = nullptr;

user_selection_t s = {
	.selected = 0,
	.machine_selected = 0,
	.header_selected = 0, 
	.asc = true,
	.search_mode = false,
	.max_machine = 1
};

void *proc_task(void*) {
	while (atomic_load(&running)) {
		if(array) proc_array_update(&array[s.machine_selected]);

		struct timespec ts = {
			.tv_sec = 0,
			.tv_nsec = 1'000'000'000 / 5// 1/10 seconde
		};

		nanosleep(&ts, nullptr);

	}
	return nullptr;
}

void *ui_task(void*) {
	ui_main(array, &s);
	atomic_store(&running, false);
	return nullptr;
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


	array = malloc(s.max_machine * sizeof(*array));
	if(!array) return EXIT_FAILURE;

	pthread_t proc_thread, ui_thread;
	pthread_create(&proc_thread, nullptr, proc_task, nullptr);
	pthread_create(&ui_thread, nullptr, ui_task, nullptr);

	pthread_join(proc_thread, nullptr);
	pthread_join(ui_thread, nullptr);

	for(size_t i = 0; i < s.max_machine; ++i) {
		proc_array_free(&array[i]);
	}
	free(array);
}

