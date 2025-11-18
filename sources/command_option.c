#include "command_option.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void print_help(){
	printf(
    "-h ou --help : affiche l'aide du programme ainsi que la syntaxe d'exécution du programme\n"
    "--dry-run : test l'accès à la liste des processus sur la machine locale et/ou distante sans les afficher\n"
    "-c ou --remote-config : spécifie le chemin vers le fichier de configuration contenant les informations de connexion sur les machines distantes\n"
    "-t ou --connexion-type : spécifie le type de connexion à utiliser pour la connexion sur les machines distantes (ssh, telnet)\n"
    "-P ou --port : spécifie le port à utiliser pour le type de connexion choisi.
    Si cette option n'est pas spécifié, alor le port par défaut du type de connexion est choisi\n"
    "-l ou --login : spécifie l'identifiant de connexion et la machine distante.
    Ex : --login user@remote_server. remote_server est soit l'IP ou le nom DNS de la machine distante\n"
    "-s ou --remote-server : spécifie le nom DNS ou l'IP de la machine distante\n"
    "-u ou --username : spécifie le nom de l'utilisateur à utiliser pour la connexion\n"
    "-p ou --password : spécifie le mot de passe à utiliser pour la connexion\n"
    "-a ou --all : spécifie au programme de collecter à la fois la liste des processus sur la machine local et les machines distantes.
    S'utilise uniquement si l'option -c ou -s est utilisé.\n");
}

int command_run(int argc, char *argv[]){
	int opt = 0;
	struct option my_opts[] = {
		{.name="help",.has_arg=0,.flag=0,.val='h'},
		{.name="dry-run",.has_arg=0,.flag=0,.val=129},
		{.name="remote-config",.has_arg=1,.flag=0,.val='c'},
		{.name="connexion-type",.has_arg=1,.flag=0,.val='t'},
		{.name="port",.has_arg=1,.flag=0,.val='P'},
		{.name="login",.has_arg=1,.flag=0,.val='l'},
		{.name="remote-server",.has_arg=1,.flag=0,.val='s'},
		{.name="username",.has_arg=1,.flag=0,.val='u'},
		{.name="password",.has_arg=1,.flag=0,.val='p'},
		{.name="all",.has_arg=1,.flag=0,.val='a'},
		{.name=0,.has_arg=0,.flag=0,.val=0},
	};
	while((opt = getopt_long(argc, argv, "", my_opts, NULL)) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				break;
				
			case 129:
				break;
				
			case 'c':
				break;

			case 't':
				break;

			case 'P':
				break;

			case 'l':
				break;

			case 's':
				break;
			
			case 'u':
				break;
			
			case 'p':
				break;
			
			case 'a':
				break;

		}
	}
	return 0;
}
