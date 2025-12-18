#ifndef THREAD_H
#define THREAD_H

#include <stdatomic.h>
#include <pthread.h>
#include "processus_array.h"
#include "ui_constant.h"
#include "ui.h"

typedef struct double_buffer_t {
	processus_array_t	buffer[2];
	atomic_int		active;
} double_buffer_t;

typedef struct thread_args_t {
	double_buffer_t *array;
	user_selection_t selection;
	atomic_bool running;
} thread_args_t;

void *proc_task(void *arg);
void *ui_task(void *arg);

[[nodiscard]] error_code_t init_data(thread_args_t *args);
[[nodiscard]] error_code_t start_threads(thread_args_t *args);
void release_data(thread_args_t *args);
[[nodiscard]] error_code_t bottom_ui(thread_args_t *args);

#endif /* THREAD_H */
