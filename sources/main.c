#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

error_code_t thread_args_init(thread_args_t *args) {

	if (!args) {
		return NULLPTR_PARAMETER_ERROR;
	}

	args->array = nullptr;
	args->exec_local = false;
	args->selection.selected = 0;
	args->selection.machine_selected = 0;
	args->selection.header_selected = 0;
	args->selection.sort = ASC;
	args->selection.mode = NORMAL;
	args->selection.event = NOTHING;
	args->selection.input[0] = '\0';
	args->selection.input_length = 0;	
	args->selection.max_machine = 0;	
	args->selection.indices.data = nullptr;
	args->selection.indices.size = 0;
	args->selection.indices.capacity = 0;
	atomic_store_explicit(&args->running, true, memory_order_release);
	ssh_array_init(&args->sessions);
	if (pthread_mutex_init(&args->selection.lock, nullptr) != 0) {
		return MEMORY_ALLOCATION_FAILED;
	}

	return SUCCESS;
}

error_code_t machine_array_init(thread_args_t *args) {
	
	if (!args) {
		return NULLPTR_PARAMETER_ERROR;
	}

	args->array = calloc(args->selection.max_machine, sizeof(*args->array));
	if (!args->array) {
		return MEMORY_ALLOCATION_FAILED;
	}

	for (size_t i=0; i<args->selection.max_machine; ++i) {
		proc_array_init(&args->array[i].buffer[0]);
		proc_array_init(&args->array[i].buffer[1]);

		atomic_store_explicit(&args->array[i].active, 0, memory_order_release);
	}

	return SUCCESS;
}

int main(int argc, char *argv[]) {	
	
	pthread_t ui_thread;
	pthread_t proc_thread;
	
	flag_t flag;
	flag_init(&flag);

	thread_args_t args;
	thread_args_init(&args);

	error_code_t err = command_run(argc, argv, &flag, &args.sessions);
	if (err != SUCCESS) {
		return err;
	}

	args.exec_local = flag.exec_local;
	args.selection.max_machine = args.sessions.size + flag.exec_local;
	
	err = machine_array_init(&args);
	if (err != SUCCESS) {
		release_data(&args);
		return err;
	}
	
	if (flag.exec_local) {
	  	if (pthread_create(&proc_thread, nullptr, proc_task, &args) != 0) {
	    	return THREAD_FAILED;
		}
	}
	//if(sessions.size != 0)	// start ssh thread
	
	//	run ui task
	if (pthread_create(&ui_thread, nullptr, ui_task, &args) != 0) {
		atomic_store_explicit(&args.running, false, memory_order_release);

		if (flag.exec_local) {
			pthread_join(proc_thread, nullptr);
		}

		return THREAD_FAILED;
	}
	
	pthread_join(ui_thread, nullptr);
   	if (flag.exec_local) {
		pthread_join(proc_thread, nullptr);
	}
	//	printf("\n\nmax machines: %ld\n", args.selection.max_machine);
	//	return (bottom_ui(&args) != SUCCESS) ? EXIT_FAILURE : EXIT_SUCCESS;
	
	release_data(&args); 
	ssh_array_free(&sessions);
	return err;
}

