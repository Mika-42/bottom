#include "config_file_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>

void cfg_init(config_file_t *file) {
	if(!file) return;
	file->data = nullptr;
	file->size = 0;
	file->capacity = 0;
}

remote_server_t *cfg_add(config_file_t *file, remote_server_t *server) {
	
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

void cfg_free(config_file_t *file) {
	if(!file) return;
	if(file->data) free(file->data);
	cfg_init(file);
}

error_code_t cfg_parse_line(char *line, remote_server_t *server) {
	
	if(!line || !server) return NULLPTR_PARAMETER_ERROR;

	memset(server, 0, sizeof(*server));
	
	char *token = strtok(line, ":");
	if(!token) return READ_CFG_SERVER_NAME_FAILED;
	strncpy(server->server_name, token, field_size - 1);
		
	token = strtok(nullptr, ":");
	if(!token) return READ_CFG_SERVER_ADDR_FAILED;
	strncpy(server->server_addr, token, field_size - 1);
		
	token = strtok(nullptr, ":");
	if(!token) return READ_CFG_PORT_FAILED;
	char *endptr;
	long port = strtol(token, &endptr, 10);
	if(*endptr != '\0' || port < 0 || (size_t)port > max_port) return INVALID_NUMERIC_DATA;
	server->port = (unsigned short)port;
   
	token = strtok(nullptr, ":");
	if(!token) return READ_CFG_USERNAME_FAILED;
	strncpy(server->username, token, field_size - 1);
   
	token = strtok(nullptr, ":");
	if(!token) return READ_CFG_PASSWORD_FAILED;
	strncpy(server->password, token, field_size - 1);
   
	token = strtok(nullptr, ":");
	if(!token) return READ_CFG_CONNEXION_FAILED;
   
	if (strcasecmp(token, "ssh") == 0) server->connexion = SSH;
	else if (strcasecmp(token, "telnet") == 0) server->connexion = TELNET;
	else return INVALID_CONNEXION_TYPE;

	return SUCCESS;
}

bool is_cfg_file(const char *path) {
	if (!path) return false;
	const char *basename = strrchr(path, '/');

	if (basename)
        basename++; // passe le '/'
    else
        basename = path;

    return strcmp(basename, cfg_filename) == 0;
}


bool path_exist(const char *path) {
	if(!path) return false;
	return access(path, F_OK) == 0;
}

error_code_t cfg_get_valid(char* path) {

	if(!path) return NULLPTR_PARAMETER_ERROR;

	if(!path_exist(path)) {
		if(!path_exist("./.config")) return FILE_DOES_NOT_EXIST;
		strncpy(path, "./.config",path_max_size - 1);
		path[path_max_size - 1] = '\0';
	}

	if(!is_cfg_file(path)) return UNREGULAR_FILE_ERROR;

	struct stat st;
	if(stat(path, &st) != 0) return FILE_DOES_NOT_EXIST;

	if (!S_ISREG(st.st_mode)) return UNREGULAR_FILE_ERROR;

	mode_t perms = st.st_mode & 0777;
    if (perms != 0600) return BAD_PERMISSIONS_ERROR;
	
	return SUCCESS;
}

error_code_t cfg_load_file(config_file_t *file, char *path) {

	error_code_t err = cfg_get_valid(path);
	if(err != SUCCESS) return err;

	FILE *f = fopen(path, "r");
	if(!f) {
		perror("fopen");
		return OPEN_FILE_FAILED;
	}
	char line[1024];
	remote_server_t server;

	while (fgets(line, sizeof(line), f)) {
	    line[strcspn(line, "\n")] = '\0';
		
		err = cfg_parse_line(line, &server);
	   
		if(err != SUCCESS) {
			fclose(f);
			return err;
		}
		cfg_add(file, &server); 
	}
	fclose(f);

	return SUCCESS;
}

