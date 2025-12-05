#ifndef PROCESSUS_ARRAY_H
#define PROCESSUS_ARRAY_H

#include "processus.h"

typedef struct processus_array_t {
	processus_t	*data;
	size_t		size;
} processus_array_t;

void proc_array_push(processus_array_t *array, const processus_t *proc);

#endif //PROCESSUS_ARRAY_H
