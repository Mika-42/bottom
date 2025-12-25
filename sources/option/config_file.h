#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <stddef.h>
#include "remote_server.h"

constexpr size_t path_max_size = 4096;
constexpr char cfg_filename[path_max_size] = ".config";

typedef struct config_file_t {
	remote_server_t *data;
	size_t size;
	size_t capacity;
} config_file_t;

void cfg_init(config_file_t *file);
void cfg_free(config_file_t *file);
remote_server_t *cfg_add_server(config_file_t *file, remote_server_t *server);

bool cfg_is_dot_config(const char *path);
error_code_t cfg_is_600(const char *path);

error_code_t cfg_parse(config_file_t *file, const char *path);

#endif /* CONFIG_FILE_H */
