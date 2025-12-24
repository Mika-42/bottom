#include "command_option.h"
#include "config_file.h"
#include "error.h"
#include "option.h"
#include "ssh_connexion.h"

#include <getopt.h>
#include <stdio.h>

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const struct option long_opts[11] = {
    {.name = "help", .has_arg = 0, .flag = 0, .val = 'h'},
    {.name = "dry-run", .has_arg = 0, .flag = 0, .val = 129},
    {.name = "remote-config", .has_arg = 1, .flag = 0, .val = 'c'},
    {.name = "connexion-type", .has_arg = 1, .flag = 0, .val = 't'},
    {.name = "port", .has_arg = 1, .flag = 0, .val = 'P'},
    {.name = "login", .has_arg = 1, .flag = 0, .val = 'l'},
    {.name = "remote-server", .has_arg = 1, .flag = 0, .val = 's'},
    {.name = "username", .has_arg = 1, .flag = 0, .val = 'u'},
    {.name = "password", .has_arg = 1, .flag = 0, .val = 'p'},
    {.name = "all", .has_arg = 0, .flag = 0, .val = 'a'},
    {.name = 0, .has_arg = 0, .flag = 0, .val = 0},
};

struct {
	bool exec_local : 1;
	bool config : 1;
	bool server : 1;
	bool has_opt : 1;
	bool dry_run : 1;
} flag = {false, false, false, false, false};

error_code_t ask_user(const char *field_name, char *dest) {

  if (!field_name || !dest)
    return NULLPTR_PARAMETER_ERROR;

  printf("%s: ", field_name);

  if (scanf("%256s", dest) != 1) {
    getchar();
    return BUFFER_OVERFLOW;
  }
  getchar();

  return SUCCESS;
}

int command_run(int argc, char *argv[]) {
  int opt = 0;
  config_file_t cfg_file = {0};
  remote_server_t server = {0};
  error_code_t err = SUCCESS;
  optind = 1;
	

  while ((opt = getopt_long(argc, argv, "hc::t:P:l:s:u:p:a", long_opts,
                            nullptr)) != -1) {
    flag.has_opt = true;
	switch (opt) {

    case 'h': {
      opt_print_help();
      return SPECIAL_EXIT;
			  }

    case 129: { 
				  flag.dry_run = true;
			  }
      break;

    case 'c': {
      err = cfg_parse(&cfg_file, optarg);
	  flag.config = true;
    } break;
    
	case 't': {
      err = srv_str_to_connexion(&server.connexion, optarg);
    } break;

    case 'P': {
      err = srv_str_to_uint16(&server.port, optarg);
    } break;

    case 'l': {
      err = srv_str_split_at(server.username, server.address, optarg);
    } break;

    case 's': {
      err = srv_str_duplicate(server.address, optarg);
		flag.server = true;
	  } break;

    case 'u': {
      err = srv_str_duplicate(server.username, optarg);
    } break;

    case 'p': {
      err = srv_str_duplicate(server.password, optarg);
    } break;

	case 'a': {
				  flag.exec_local = true;
			  } break;
    case '?':
      return PARSING_FAILED;
      break;
    }

    if (err != SUCCESS) {
      fprintf(stderr, "Error: %s (%s).\n", err_to_str(err), optarg);
      return err;
    }
  }

  if(flag.exec_local && !(flag.server || flag.config)) {
		  err = INVALID_ARGUMENT;
			fprintf(stderr, "Error: %s (--all/-a requires --remote-config/-c or -remote-server/-s.).\n", err_to_str(err));		
		  return err;
	}

  if (!srv_str_is_empty(server.address)) {
    if (srv_str_is_empty(server.username)) {
      err = ask_user("username", server.username);
      if (err != SUCCESS)
        return err;
    }
    if (srv_str_is_empty(server.password)) {
      err = ask_user("password", server.password);
      if (err != SUCCESS)
        return err;
    }

	cfg_add_server(&cfg_file, &server);
  }
 	
  if (flag.dry_run) {
	for(size_t i = 0; i < cfg_file.size; ++i) {
		ssh_session session = ssh_connexion_init(
				cfg_file.data[i].address,
				cfg_file.data[i].port,
				cfg_file.data[i].username,
				cfg_file.data[i].password
				);
		if(!session) {
			err = SSH_CONNEXION_FAILED;
			fprintf(stderr, "Error: %s [%s](%s@%s).\n",
					err_to_str(err),
					cfg_file.data[i].name,
					cfg_file.data[i].username, 
					cfg_file.data[i].address);
			continue;
		}

		bool succed = ssh_dry_run(session) == SUCCESS;

		printf("Testing connexion on [%s](%s@%s) : %s.\n",
					cfg_file.data[i].name,
					cfg_file.data[i].username, 
					cfg_file.data[i].address,
					succed ? "succed" : "failed"
					);
		ssh_end_session(session);
	}
	return SPECIAL_EXIT;
  }
	  
if (!flag.has_opt) {
	flag.exec_local = true;
  }

  return 0;
}
