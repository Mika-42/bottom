#include "command_option.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static void print_help();
static int handle_login_option(options_prog *options);
static int check_config_file_permissions(options_prog *options);


static void print_help(){
	const char* help_message = "-h ou --help : affiche l'aide du programme ainsi que la syntaxe d'exécution du programme\n"
   	 "--dry-run : test l'accès à la liste des processus sur la machine locale et/ou distante sans les afficher\n"
   	 "-c ou --remote-config : spécifie le chemin vers le fichier de configuration contenant les informations de connexion sur les machines distantes\n"
   	 "-t ou --connexion-type : spécifie le type de connexion à utiliser pour la connexion sur les machines distantes (ssh, telnet)\n"
   	 "-P ou --port : spécifie le port à utiliser pour le type de connexion choisi. \
   	 Si cette option n'est pas spécifié, alor le port par défaut du type de connexion est choisi\n"
   	 "-l ou --login : spécifie l'identifiant de connexion et la machine distante. \
   	 Ex : --login user@remote_server. remote_server est soit l'IP ou le nom DNS de la machine distante\n"
   	 "-s ou --remote-server : spécifie le nom DNS ou l'IP de la machine distante\n"
   	 "-u ou --username : spécifie le nom de l'utilisateur à utiliser pour la connexion\n"
   	 "-p ou --password : spécifie le mot de passe à utiliser pour la connexion\n"
   	 "-a ou --all : spécifie au programme de collecter à la fois la liste des processus sur la machine local et les machines distantes. \
   	 S'utilise uniquement si l'option -c ou -s est utilisé.\n";

	printf("%s", help_message);
}

static int handle_login_option(options_prog *options) {
    if (options->login != NULL) {
        char *separator = strchr(options->login, '@');
        
        if (separator == NULL) {
            fprintf(stderr, "Erreur: Le format de l'option -l doit etre 'username@remote_server'.\n");
            return -1;
        }

        options->remote_server = strdup(separator + 1);
        if (options->remote_server == NULL) {
            perror("Erreur d'allocation memoire (remote_server)");
            return -1;
        }

        size_t username_len = separator - options->login;
        options->username = (char *)malloc(username_len + 1);
        if (options->username == NULL) {
            perror("Erreur d'allocation memoire (username)");
            free(options->remote_server);
            options->remote_server = NULL;
            return -1;
        }

        strncpy(options->username, options->login, username_len);
        options->username[username_len] = '\0';
    }
    return 0;
}

static int check_config_file_permissions(options_prog *options) {
    if (options->remote_config != NULL) {
        struct stat file_stat;
        
        if (stat(options->remote_config, &file_stat) == -1) {
            fprintf(stderr, "Erreur: Impossible d'acceder au fichier de configuration '%s'. %s\n", 
                    options->remote_config, strerror(errno));
            return -1;
        }

        if ((file_stat.st_mode & 0777) != 0600) {
            fprintf(stderr, "Erreur: Le fichier de configuration '%s' doit avoir les permissions 600 (rw-------).\n", 
                    options->remote_config);
            return -1;
        }
    }
    return 0;
}


int command_run(int argc, char *argv[], options_prog *options){
	int opt = 0;
	struct option long_opts[] = {
		{.name="help",.has_arg=0,.flag=0,.val='h'},
		{.name="dry-run",.has_arg=0,.flag=0,.val=129},
		{.name="remote-config",.has_arg=1,.flag=0,.val='c'},
		{.name="connexion-type",.has_arg=1,.flag=0,.val='t'},
		{.name="port",.has_arg=1,.flag=0,.val='P'},
		{.name="login",.has_arg=1,.flag=0,.val='l'},
		{.name="remote-server",.has_arg=1,.flag=0,.val='s'},
		{.name="username",.has_arg=1,.flag=0,.val='u'},
		{.name="password",.has_arg=1,.flag=0,.val='p'},
		{.name="all",.has_arg=0,.flag=0,.val='a'},
		{.name=0,.has_arg=0,.flag=0,.val=0},
	};
    
    optind = 1; 

	while((opt = getopt_long(argc, argv, "hc:t:P:l:s:u:p:a", long_opts, NULL)) != -1) {
		switch (opt) {
			case 'h':
				options->help = true;
				print_help();
				exit(0);
				break;
				
			case 129:
				options->dry_run = true;
				break;
				
			case 'c':
				options->remote_config = optarg;
				break;

			case 't':
				options->connexion_type =  optarg;
				break;

			case 'P':
				options->port = atoi(optarg); 
				break;

			case 'l':
				options->login = optarg;
				break;

			case 's':
				options->remote_server = optarg;
				break;
			
			case 'u':
				options->username = optarg;
				break;
			
			case 'p':
				options->password = optarg;
				break;
			
			case 'a':
				options->all = true;
				break;
			case '?':
				return -1;
				break;
		}
	}
    
    if (handle_login_option(options) != 0) {
        return -1;
    }

	if (options->port == 0 && options->connexion_type != NULL) {
		if (strcmp(options->connexion_type, "ssh") == 0){
			options->port = 22;
		}
		else if (strcmp(options->connexion_type, "telnet") == 0){
			options->port = 23;
		}
	}

	if (options->remote_config == NULL && options->remote_server == NULL) {
		options->all = false;
	}

	if (options->remote_config == NULL) {
		DIR *d = opendir(".");
        if (d != NULL) {
            for (struct dirent *element; (element = readdir(d)) != NULL; ) {
                if (strcmp(element->d_name, ".config") == 0) {
                    options->remote_config = ".config"; 
                    break;
                }
            }
            closedir(d);
        }
	}
    
    if (check_config_file_permissions(options) != 0) {
        return -1;
    }
	
	return 0;
}