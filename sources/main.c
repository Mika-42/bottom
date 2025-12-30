#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "command_option.h"
#include "thread.h"

error_code_t flag_init(flag_t *flag) {
	if (!flag) {
		return NULLPTR_PARAMETER_ERROR;
	}

	flag->exec_local = false;
	flag->config = false;
	flag->server = false;
	flag->has_opt = false;
	flag->dry_run = false;

	return SUCCESS;
}

int main(int argc, char **argv) {

	user_selection_t selection = {

		.selected = 0,
		.machine_selected = 0,
		.header_selected = 0,
		.sort = ASC,
		.mode = NORMAL,
		.event = NOTHING,
		.input[0] = '\0',
		.input_length = 0,
		.indices = { 
			.data = nullptr,
			.size = 0,
			.capacity = 0
		}
	};

	pthread_t ui_thread;
	pthread_t manager_thread;

	thread_args_t *args;
	double_buffer_t *array;   

	pthread_t *worker_threads;

	flag_t flag = {0};
	config_file_t cfg_file = {0};

	ssh_session_array_t sessions;
	ssh_array_init(&sessions);
	error_code_t err = command_run(argc, argv, &flag, &sessions, &cfg_file);

	if (err != SUCCESS) {
		return err;
	}

	if (pthread_mutex_init(&selection.lock, nullptr) != 0) {
		return MEMORY_ALLOCATION_FAILED;
	}

	const size_t max_machine = sessions.size + (flag.exec_local ? 1 : 0);
	selection.max_machine = max_machine;

	args = calloc(max_machine, sizeof(thread_args_t));
	worker_threads = calloc(max_machine, sizeof(pthread_t));
	array = calloc(max_machine, sizeof(double_buffer_t));

	for(size_t i = 0; i < max_machine; ++i) {
		args[i].selection = &selection;
		args[i].array = &array[i];

		char *machine_name = args[i].machine_name;
			if (flag.exec_local) {
				if (i == 0) {
					args[i].session = nullptr; // machine locale
					strncpy(machine_name, "localhost", 32);
					machine_name[31] = '\0';

				} else {
					args[i].session = sessions.data[i - 1]; // SSH
				}
			} else {
				args[i].session = sessions.data[i];
			}

		if(args[i].session) {
			const int shift = flag.exec_local ? 1 : 0;

			if(cfg_file.data[i - shift].name[0] != '\0') {
				char tmp[32];
				strncpy(tmp, cfg_file.data[i - shift].name, sizeof(tmp) - 1);
				tmp[sizeof(tmp) - 1] = '\0';

				snprintf(machine_name, 32, "%s", tmp);

			} else {

				char username_part[10]; // moitié de 31
				char address_part[20];
				strncpy(username_part, cfg_file.data[i - shift].username, sizeof(username_part) - 1);
				username_part[sizeof(username_part) - 1] = '\0';

				 strncpy(address_part, cfg_file.data[i - shift].address, sizeof(address_part) - 1);
				address_part[sizeof(address_part) - 1] = '\0';

				snprintf(machine_name, 32, "%s@%s", username_part, address_part);
			}

		}

		if (pthread_create(&worker_threads[i], nullptr, proc_task, &args[i]) != 0) {

			for(size_t j=0; j<i; ++j) pthread_join(worker_threads[j], nullptr);

			return THREAD_FAILED;
		}
	}

	//	manager ui task
	if (pthread_create(&manager_thread, nullptr, manager_task, &args[0]) != 0) {

		for(size_t i=0; i<max_machine; ++i){	
			pthread_join(worker_threads[i], nullptr);	
		}
		return THREAD_FAILED;
	}

	//	run ui task
	if (pthread_create(&ui_thread, nullptr, ui_task, &args[0]) != 0) {

		for(size_t i=0; i<max_machine; ++i){	
			pthread_join(worker_threads[i], nullptr);	
		}

		pthread_join(manager_thread, nullptr);

		return THREAD_FAILED;
	}
	
	pthread_join(ui_thread, nullptr);

	pthread_join(manager_thread, nullptr);
	for(size_t i=0; i<max_machine; ++i) {
		pthread_join(worker_threads[i], nullptr);		
	}

	if (array) free(array);

	pthread_mutex_destroy(&selection.lock);
	ui_index_array_free(&selection.indices);
	ssh_array_free(&sessions);

	return 0;
}
