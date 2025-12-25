#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include <stddef.h>     
#include "processus_array.h" 

int process_to_string(const processus_t *proc, char *buffer, size_t max_size);
char *serialize_process_list(processus_array_t *array);
int send_data(int socket_fd, processus_array_t *array);

#endif
