#include "command_option.h"
#include "error.h"
#include "ssh_connexion.h"

#include <getopt.h>
#include <stdio.h>

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const struct option long_opts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"dry-run", no_argument, nullptr, 129},
    {"remote-config", optional_argument, nullptr, 'c'},
    {"connexion-type", required_argument, nullptr, 't'},
    {"port", required_argument, nullptr, 'P'},
    {"login", required_argument, nullptr, 'l'},
    {"remote-server", required_argument, nullptr, 's'},
    {"username", required_argument, nullptr, 'u'},
    {"password", required_argument, nullptr, 'p'},
    {"all", no_argument, nullptr, 'a'},
    {0, 0, 0, 0}};

error_code_t command_run(int argc, char *argv[], flag_t *flag, ssh_session_array_t *sessions, config_file_t *cfg_file) {
    int opt = 0;
    remote_server_t server = {0};
    error_code_t err = SUCCESS;
    optind = 1;

    while ((opt = getopt_long(argc, argv, "hc::t:P:l:s:u:p:a", long_opts,nullptr)) != -1) {
		flag->has_opt = true;

		switch (opt) {

			case 'h': {
				opt_print_help();
				return SPECIAL_EXIT;
			} 

			case 129: {
				flag->dry_run = true;
			} break;

			case 'c': {
				err = cfg_parse(cfg_file, optarg);
				flag->config = true;
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
				flag->server = true;
			} break;

			case 'u': {
				err = srv_str_duplicate(server.username, optarg);
			} break;

			case 'p': {
				err = srv_str_duplicate(server.password, optarg);
			} break;

			case 'a': {
				flag->exec_local = true;
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

	if (flag->exec_local && !(flag->server || flag->config)) {
		err = INVALID_ARGUMENT;
		fprintf(stderr,
				"Error: %s (--all/-a requires --remote-config/-c or "
				"-remote-server/-s.).\n",
				err_to_str(err));
		return err;
	}

	if (!srv_str_is_empty(server.address)) {
		if (srv_str_is_empty(server.username)) {
			err = opt_ask_user("username", server.username);
			if (err != SUCCESS) {
				return err;
			}
		}
		if (srv_str_is_empty(server.password)) {
			err = opt_ask_user("password", server.password);
			if (err != SUCCESS) {
				return err;
			}
		}

		cfg_add_server(cfg_file, &server);
	}

	if (flag->dry_run) {
		opt_dry_run(cfg_file);
		return SPECIAL_EXIT;
	}

	if (!flag->has_opt) {
		flag->exec_local = true;
	}

	err = opt_connect(cfg_file, sessions);
	if (err != SUCCESS) {
		fprintf(stderr, "Error: %s.\n", err_to_str(err));
	}

	return err;
	}
