#include "processus_array.h"
#include "processus_signal.h"
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>

void proc_array_init(processus_array_t *array) {
	array->data = nullptr;
	array->size = 0;
	array->capacity = 0;
	array->cpu_tick = 0;
	array->boot_time = 0;
}

processus_t *proc_array_get_last(processus_array_t *array) {
	return (!array || array->size == 0 || !array->data) ? 
		nullptr : &array->data[array->size - 1];
}

processus_t *proc_array_emplace_back(processus_array_t *array) {

	if (!array) return nullptr;

	if (array->size >= array->capacity) {
		const size_t new_capacity = array->capacity ? array->capacity * 2 : 1;

		processus_t *temp = realloc(array->data, new_capacity * sizeof(*temp));

		if (!temp) return nullptr;

		array->data = temp;
		array->capacity = new_capacity;
	}

	array->data[array->size++] = (processus_t){0};

	return proc_array_get_last(array);	
}

void proc_array_free(processus_array_t *array) {

	if (!array) return;

	free(array->data);
	array->data = nullptr;
	array->size = 0;
	array->capacity = 0;
}

bool wrapper(processus_t* proc) { 
	return pid_does_not_exists(proc->pid); 
}	

error_code_t proc_array_update(processus_array_t* array) {

	if (!array) return NULLPTR_PARAMETER_ERROR;

	DIR *rep_proc = opendir("/proc");
	if (!rep_proc) return OPEN_FILE_FAILED;

	struct dirent *ent = nullptr;

	proc_array_remove_if(array, wrapper);

	while ((ent = readdir(rep_proc))) {
		if (!proc_is_valid_pid(ent->d_name)) continue;

		char *end = nullptr;
		const pid_t pid = strtol(ent->d_name, &end, 10);
		if (*end != '\0' || pid <= 0) continue;

		processus_t *proc = proc_array_find_by_pid(array, pid);

		if (!proc) {
			proc = proc_array_emplace_back(array);

			if (!proc) {
				closedir(rep_proc);
				return MEMORY_ALLOCATION_FAILED;
			}
		}

		if (proc_get_all_infos(pid, proc) != SUCCESS) {
			processus_t *last = proc_array_get_last(array);

			// on écrase le proc mort avec le dernier proc de la liste
			if (proc != last) *proc = *last;

			// on retire le dernier proc
			array->size--;
		}
	}

	closedir(rep_proc);

	return proc_get_global_stat(&array->cpu_tick, &array->boot_time);
}


processus_t *proc_array_find_by_pid(const processus_array_t *array, const pid_t pid) {

	if (!array) return nullptr;

	for (size_t i=0; i<array->size; i++) {
		processus_t *e = &array->data[i];
		if (e->pid == pid) return e;
	}

	return nullptr;
}

void proc_array_remove_if(processus_array_t *array, proc_predicate_t pred) {

	//iterators
	processus_t *write = array->data;
	processus_t *read = array->data;
	processus_t *end = array->data + array->size;

	for (; read!=end; ++read) {
		if (!pred(read)) *write++ = *read;
	}

	array->size = (size_t)(write - array->data);
}

error_code_t proc_array_get_cpu(const processus_array_t *prev_array, processus_array_t *current_array) {
	
	if(!prev_array || !current_array) return NULLPTR_PARAMETER_ERROR;

	const size_t d_machine_time = current_array->cpu_tick - prev_array->cpu_tick;
	
	if (d_machine_time == 0) return SUCCESS;
	
	for(size_t i = 0; i < current_array->size; ++i) {
		processus_t *current_proc = &current_array->data[i];
		const processus_t *prev_proc = proc_array_find_by_pid(prev_array, current_proc->pid);
		
		if(!prev_proc) {
			current_proc->cpu_usage = 0;
			continue;
		}

		const size_t current_proc_time = current_proc->stime + current_proc->utime;
		const size_t prev_proc_time = prev_proc->stime + prev_proc->utime;
		const size_t d_proc_time = current_proc_time - prev_proc_time;
		
		const double cpu_usage = ((double)d_proc_time / (double)d_machine_time) * 100.0 * 16.0; //todo get nb cores
		
		const double alpha = 0.01;
		current_proc->cpu_usage = alpha * cpu_usage + (1.0 - alpha) * current_proc->cpu_usage;
	}
	return SUCCESS;
}
