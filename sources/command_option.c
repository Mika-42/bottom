#include "command_option.h"
#include "config_file.h"
#include "error.h"
#include "option.h"

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
  // config_file_t cfg_file = {0};
  remote_server_t server = {0};
  error_code_t err = SUCCESS;
  optind = 1;

  while ((opt = getopt_long(argc, argv, "hc::t:P:l:s:u:p:a", long_opts,
                            nullptr)) != -1) {
    switch (opt) {

    case 'h':
      opt_print_help();
      return 0;

    case 129:
      //      options->dry_run = true;
      break;

    case 'c': {
      //     err = opt_remote_cfg(&cfg_file, optarg);
      //   if (err != SUCCESS)
      //   return err;
    } break;
      //-------------------
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
    } break;

    case 'u': {
      err = srv_str_duplicate(server.username, optarg);
    } break;

    case 'p': {
      err = srv_str_duplicate(server.password, optarg);
    } break;
      //--------------------------------------------
    case 'a':
      // options->all = true;
      break;
    case '?':
      return PARSING_FAILED;
      break;
    }

    if (err != SUCCESS) {
      fprintf(stderr, "Error: %s (%s).\n", err_to_str(err), optarg);
      return err;
    }
  }

  // -s
  if (!srv_str_is_empty(server.address)) {
    if (srv_str_is_empty(server.username)) {
      err = ask_user("username", server.username);
	  if(err != SUCCESS) return err;
    }
    if (srv_str_is_empty(server.password)) {
      err = ask_user("password", server.password);
	  if(err != SUCCESS) return err;
	}
  }
  //------
  /*
          if (options->port == 0 && !is_empty(options->connexion_type)) {
      if (strcmp(options->connexion_type, "ssh") == 0) {
        options->port = 22;
      } else if (strcmp(options->connexion_type, "telnet") == 0) {
        options->port = 23;
      }
          }

          if (is_empty(options->remote_config) &&
     is_empty(options->remote_server)) { options->all = false;
          }

          if (is_empty(options->remote_config) &&
     is_empty(options->remote_server)) { if (!is_empty(options->username) ||
     !is_empty(options->password) || !is_empty(options->connexion_type) ||
     options->port != 0) { fprintf(stderr, "Erreur: Les options de connexion
     (-u, -p, -t, -P) ne " "sont valides qu'avec -s, -l ou -c.\n"); return
     PARSING_FAILED;
      }
          }

          if (is_empty(options->remote_config)) {
      DIR *d = opendir(".");
      if (d != NULL) {
        for (struct dirent *element; (element = readdir(d)) != NULL;) {
          if (strcmp(element->d_name, ".config") == 0) {
            copy_option_arg(options->remote_config, ".config");
            break;
          }
        }
        closedir(d);
      }
          }

          FILE *f = NULL;

          if (!is_empty(options->remote_config)) {
      f = fopen(options->remote_config, "r");
          }

          if (!is_empty(options->remote_config) && f == NULL) {
      fprintf(stderr, "Le fichier %s n'a pas pu être ouvert\n",
              options->remote_config);
      return OPEN_FILE_FAILED;
          }

          if (f != NULL) {
      options->nb_hosts = 0;

      while (options->nb_hosts < 10 &&
             fscanf(f, "%255[^:]:%255[^:]:%d:%255[^:]:%255[^:]:%9[^\n]\n",
                    options->hosts[options->nb_hosts].name_server,
                    options->hosts[options->nb_hosts].adress_server,
                    &options->hosts[options->nb_hosts].port,
                    options->hosts[options->nb_hosts].username,
                    options->hosts[options->nb_hosts].password,
                    options->hosts[options->nb_hosts].type_co) == 6) {
        options->nb_hosts++;
      }
      fclose(f);
          }

          struct stat fichier_stat;
          if (!is_empty(options->remote_config)) {
      if (stat(options->remote_config, &fichier_stat) == -1) {
        fprintf(
            stderr,
            "Erreur: Impossible d'acceder au fichier de configuration '%s'.
     %s\n", options->remote_config, strerror(errno)); return OPEN_FILE_FAILED;
      }
      if ((fichier_stat.st_mode & 0777) != 0600) {
        fprintf(stderr,
                "Erreur: Le fichier de configuration '%s' doit avoir les "
                "permissions 600 ( rw-------)\n",
                options->remote_config);
        return READ_FAILED;
      }
          }

          if (is_empty(options->password) && !is_empty(options->username) &&
                          (!is_empty(options->remote_server) ||
                           !is_empty(options->remote_config))) {
      if (fgets(options->password, MAX_LENGTH, stdin) != NULL) {
        options->password[strcspn(options->password, "\n")] = '\0';
      }
          }
  */
  return 0;
}
