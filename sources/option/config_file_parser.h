#ifndef CONFIG_FILE_PARSER_H
#define CONFIG_FILE_PARSER_H

#include <stddef.h>
#include "option.h"
#include "error.h"

constexpr size_t path_max_size = 4096;
constexpr char cfg_filename[path_max_size] = ".config";
constexpr size_t max_port = 65'535;

typedef struct config_file_t {
	remote_server_t *data;
	size_t size;
	size_t capacity;
} config_file_t;

void cfg_init(config_file_t *file);
remote_server_t *cfg_add(config_file_t *file, remote_server_t *server);
void cfg_free(config_file_t *file);

error_code_t cfg_parse_line(char *line, remote_server_t *server);
error_code_t cfg_get_valid(char* path);
error_code_t cfg_load_file(config_file_t *file, char *path);

#endif /* CONFIG_FILE_PARSER_H */
