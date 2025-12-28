#ifndef THREAD_H
#define THREAD_H

#include <stdatomic.h>
#include <pthread.h>
#include "processus_array.h"
#include "ui_constant.h"
#include "ssh_session_array.h"
#include "ui.h"

typedef struct double_buffer_t {
	processus_array_t buffer[2];
	atomic_int active;
} double_buffer_t;

typedef struct thread_args_t {
	double_buffer_t *array;
	user_selection_t selection;
	atomic_bool running;
	bool exec_local;
	ssh_session_array_t sessions;
} thread_args_t;

void *proc_task(void *arg);
void *ui_task(void *arg);
void *ssh_task(void *arg);

void release_data(thread_args_t *args);

#endif /* THREAD_H */
