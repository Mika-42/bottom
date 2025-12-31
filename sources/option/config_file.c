#include "config_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void cfg_init(config_file_t *file) {
  if (!file) {
    return;
  }
  file->data = nullptr;
  file->size = 0;
  file->capacity = 0;
}

void cfg_free(config_file_t *file) {
  if (!file) {
    return;
  }
  if (file->data) {
    free(file->data);
  }
  cfg_init(file);
}

remote_server_t *cfg_add_server(config_file_t *file, remote_server_t *server) {

  if (!file) {
    return nullptr;
  }
  if (file->size >= file->capacity) {
    const size_t new_capacity = file->capacity ? file->capacity * 2 : 1;

    remote_server_t *temp = realloc(file->data, new_capacity * sizeof(*temp));

    if (!temp) {
      return nullptr;
    }

    file->data = temp;
    file->capacity = new_capacity;
  }

  memcpy(&file->data[file->size], server, sizeof(remote_server_t));

  ++file->size;
  return &file->data[file->size - 1];
}

bool cfg_is_dot_config(const char *path) {

  if (!path) {
    return false;
  }
  const char *basename = strrchr(path, '/');

  if (basename) {
    basename++;
  } else {
    basename = path;
  }

  return strcmp(basename, cfg_filename) == 0;
}

error_code_t cfg_is_600(const char *path) {
	
	if (!path) {
		return NULLPTR_PARAMETER_ERROR;
	}

	struct stat st;
  if (stat(path, &st) != 0) {
    return FILE_DOES_NOT_EXIST;
  }
  if ((st.st_mode & 0777) != 0600) {
    return BAD_PERMISSIONS_ERROR;
  }
  return SUCCESS;
}

error_code_t cfg_parse(config_file_t *file, const char *path) {
  
	if (!file || !path) {
		return NULLPTR_PARAMETER_ERROR;
	}

	// open
  if (!cfg_is_dot_config(path)) {
    return DOT_CONFIG_NOT_FOUND;
  }
  error_code_t err = cfg_is_600(path);
  if (err != SUCCESS) {
    return err;
  }
  FILE *f = fopen(path, "r");
  if (!f) {

    err = cfg_is_600(cfg_filename);
    if (err != SUCCESS) {
      return err;
    }
    f = fopen(cfg_filename, "r");
    if (!f) {
      return FILE_DOES_NOT_EXIST;
    }
  }

  char line[1024];
  remote_server_t server;

  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = '\0';

    memset(&server, 0, sizeof(server));

    char *token = strtok(line, ":");
    if (!token) {
      return READ_CFG_SERVER_NAME_FAILED;
    }
    err = srv_str_duplicate(server.name, token);
    if (err != SUCCESS) {
      return err;
    }
    token = strtok(nullptr, ":");
    if (!token) {
      return READ_CFG_SERVER_ADDR_FAILED;
    }
    err = srv_str_duplicate(server.address, token);
    if (err != SUCCESS) {
      return err;
    }
    token = strtok(nullptr, ":");
    if (!token) {
      return READ_CFG_PORT_FAILED;
    }
    err = srv_str_to_uint16(&server.port, token);
    if (err != SUCCESS) {
      return err;
    }

    token = strtok(nullptr, ":");
    if (!token) {
      return READ_CFG_USERNAME_FAILED;
    }
    err = srv_str_duplicate(server.username, token);
    if (err != SUCCESS) {
      return err;
    }

    token = strtok(nullptr, ":");
    if (!token) {
      return READ_CFG_PASSWORD_FAILED;
    }
    err = srv_str_duplicate(server.password, token);
    if (err != SUCCESS) {
      return err;
    }

    token = strtok(nullptr, ":");
    if (!token) {
      return READ_CFG_CONNEXION_FAILED;
    }
    err = srv_str_to_connexion(&server.connexion, token);
    if (err != SUCCESS) {
      return err;
    }

    cfg_add_server(file, &server);
  }
  return SUCCESS;
}
