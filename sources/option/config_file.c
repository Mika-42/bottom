#include "config_file.h"
#include <stdlib.h>
#include <string.h>

void cfg_init(config_file_t *file) {
	if(!file) return;
	file->data = nullptr;
	file->size = 0;
	file->capacity = 0;
}

void cfg_free(config_file_t *file) {
	if(!file) return;
	if(file->data) free(file->data);
	cfg_init(file);
}

remote_server_t *cfg_add_server(config_file_t *file, remote_server_t *server) {
	
	if(!file) return nullptr;

	if (file->size >= file->capacity) {
		const size_t new_capacity = file->capacity ? file->capacity * 2 : 1;

		remote_server_t *temp = realloc(file->data, new_capacity * sizeof(*temp));

		if (!temp) return nullptr;

		file->data = temp;
		file->capacity = new_capacity;
	}

	memcpy(&file->data[file->size], server, sizeof(remote_server_t));

	++file->size;
	return &file->data[file->size - 1];
}

