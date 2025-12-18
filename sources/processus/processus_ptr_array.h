#ifndef PROCESSUS_PTR_ARRAY_H
#define PROCESSUS_PTR_ARRAY_H

#include "error.h"
#include <time.h>

// Forward declaration (prevent circular include)
typedef struct processus_t processus_t;

typedef struct processus_ptr_array_t {
	processus_t	**data;
	size_t		size;
	size_t		capacity;
} processus_ptr_array_t;

void		proc_ptr_array_init(processus_ptr_array_t *array);
void		proc_ptr_array_reset(processus_ptr_array_t *array);
[[nodiscard]] error_code_t proc_ptr_array_emplace_back(processus_ptr_array_t *array, processus_t *proc);
void		proc_ptr_array_free(processus_ptr_array_t *array);

#endif //PROCESSUS_PTR_ARRAY_H
