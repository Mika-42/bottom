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

error_code_t opt_ask_user(const char *field_name, char *dest) {

  if (!field_name || !dest) {
    return NULLPTR_PARAMETER_ERROR;
  }

  printf("%s: ", field_name);

  if (scanf("%256s", dest) != 1) {
    getchar();
    return BUFFER_OVERFLOW;
  }
  getchar();

  return SUCCESS;
}

error_code_t opt_dry_run(const config_file_t *file) {
	
	if (!file) {
    return NULLPTR_PARAMETER_ERROR;
  }

	for (size_t i=0; i<file->size; ++i) {
		ssh_session session = ssh_connexion_init(
			file->data[i].address,
			file->data[i].port,
			file->data[i].username,
			file->data[i].password);
   
		if (!session) {  
			fprintf(stderr, "Error: SSH_CONNEXION_FAILED [%s](%s@%s).\n",
				file->data[i].name,
				file->data[i].username, 
				file->data[i].address);
			continue;   
		}   
   
		bool succed = ssh_dry_run(session) == SUCCESS;
   
		printf("Testing connexion on [%s](%s@%s) : %s.\n",
					file->data[i].name,
					file->data[i].username, 
					file->data[i].address,
					succed ? "succed" : "failed"
					);
		ssh_end_session(session);
	}

	return SUCCESS;
}

error_code_t opt_connect(const config_file_t *file, ssh_session_array_t *array) {
	
	printf("\nConnexion...\n");
	
	for (size_t i=0; i<file->size; ++i) {
		ssh_session session = ssh_connexion_init(
			file->data[i].address,
			file->data[i].port,
			file->data[i].username,
			file->data[i].password);

		const bool succed = session != nullptr;

		if (succed) {
			if (!ssh_array_add(array, session)) {
        return MEMORY_ALLOCATION_FAILED;
			}
		}
		printf("[%s](%s@%s) : %s.\n",
			file->data[i].name,
			file->data[i].username,
			file->data[i].address,
			succed ? "succed" : "failed"
		);

	}
	return SUCCESS;
}
