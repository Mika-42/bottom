#include "option.h"

#include <string.h>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"

const char *help_message_opt = ANSI_GREEN
    "-h ou --help" ANSI_RESET
    "           : Affiche l'aide du programme ainsi que la syntaxe "
    "d'exécution du programme.\n" ANSI_GREEN "--dry-run" ANSI_RESET
    "              : Test l'accès à la liste des processus sur la "
    "machine locale et/ou distante sans les afficher.\n" ANSI_GREEN
    "-c ou --remote-config" ANSI_RESET
    "  : Spécifie le chemin vers le fichier de "
    "configuration contenant les informations de connexion sur les machines "
    "distantes.\n" ANSI_GREEN "-t ou --connexion-type" ANSI_RESET
    " : Spécifie le type de connexion à utiliser pour la "
    "connexion sur les machines distantes " ANSI_RED "(ssh, telnet)" ANSI_RESET
    ".\n" ANSI_GREEN "-P ou --port" ANSI_RESET
    "           : Spécifie le port à utiliser pour le type de "
    "connexion choisi.\n"
    "                         Si cette option n'est pas spécifié, alor le port "
    "par défaut du type de connexion est choisi.\n" ANSI_GREEN
    "-l ou --login" ANSI_RESET
    "          : Spécifie l'identifiant de connexion et la "
    "machine distante.\n" ANSI_CYAN
    "                         Ex : --login user@remote_server. remote_server "
    "est soit l'IP ou le nom DNS de la machine "
    "distante.\n" ANSI_RESET ANSI_GREEN "-s ou --remote-server" ANSI_RESET
    "  : Spécifie le nom " ANSI_RED "DNS" ANSI_RESET " ou l'" ANSI_RED
    "IP" ANSI_RESET " de la machine "
    "distante.\n" ANSI_GREEN "-u ou --username" ANSI_RESET
    "       : Spécifie le nom de l'utilisateur à utiliser pour "
    "la connexion.\n" ANSI_GREEN "-p ou --password" ANSI_RESET
    "       : Spécifie le mot de passe à utiliser pour la "
    "connexion.\n" ANSI_GREEN "-a ou --all" ANSI_RESET
    "            : Spécifie au programme de collecter à la fois la "
    "liste des processus sur la machine local et les machines distantes.\n"
    "                         S'utilise uniquement si l'option " ANSI_RED
    "-c" ANSI_RESET " ou " ANSI_RED "-s" ANSI_RESET " est "
    "utilisé.\n";

void opt_print_help() {
  FILE *pager = popen("less -R +g", "w");
  if (pager) {
    fprintf(pager, help_message_opt);
    fflush(pager);
    pclose(pager);
  }
}
/*
error_code_t opt_remote_cfg(config_file_t *file, char *opt_path) {

	char path[path_max_size] = {0};
	if(opt_path) {
		strncpy(path, opt_path, path_max_size - 1);
		path[path_max_size - 1] = '\0';
	}
	error_code_t err = cfg_load_file(file, path);

	if(err == BAD_PERMISSIONS_ERROR) {
		fprintf(stderr,
				"Configuration file must be -rw------- (600).\n"
				"Tips: chmod 600 path/to/file/.config");
		return err;
	}

	if(err == OPEN_FILE_FAILED) {
		fprintf(stderr, "You are not the owner of configuration file.\n");
		return err;
	}

	if(err == FILE_DOES_NOT_EXIST) {
		fprintf(stderr, "\"%s\" does not exist and there is no default configuration file.\n", path);
		return err;
	}

	return err;
}
*/
