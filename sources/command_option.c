#include "command_option.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static bool is_empty(const char *s) {
    return s[0] == '\0';
}

static void copy_option_arg(char *dest, const char *src) {
	size_t len = strnlen(src, MAX_LENGTH - 1);
	memcpy(dest, src, len);
	dest[len] = '\0';
}

static void print_help() {

    const char *help_message = "-h ou --help : affiche l'aide du programme ainsi que la syntaxe d'exécution du programme\n"
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

#define COPY_OPT(dest, src) \
    do { \
        strncpy(dest, src, MAX_LENGTH - 1); \
        dest[MAX_LENGTH - 1] = '\0'; \
    } while(0)

int command_run(int argc, char *argv[], options_prog *options) {
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
                copy_option_arg(options->remote_config, optarg);
                break;

            case 't':
                copy_option_arg(options->connexion_type, optarg);
                break;

            case 'P':
                options->port = atoi(optarg);
                break;

            case 'l':
                copy_option_arg(options->login, optarg);
                break;

            case 's':
                copy_option_arg(options->remote_server, optarg);
                break;
            
            case 'u':
                copy_option_arg(options->username, optarg);
                break;
            
            case 'p':
                copy_option_arg(options->password, optarg);
                break;
            
            case 'a':
                options->all = true;
                break;
            case '?':
                return -1;
                break;
        }
    }

   if (!is_empty(options->login)) {
        char login_copy[MAX_LENGTH];
        copy_option_arg(login_copy, options->login); 
        
        char *separator = strchr(login_copy, '@');
        
        if (separator == NULL) {
            fprintf(stderr, "Erreur: Le format de l'option -l doit etre 'username@remote_server'.\n");
            return -1;
        }
        copy_option_arg(options->remote_server, separator + 1);
        *separator = '\0'; 
        copy_option_arg(options->username, login_copy);
    }
    
    if (options->port == 0 && !is_empty(options->connexion_type)) {
        if (strcmp(options->connexion_type, "ssh") == 0){
            options->port = 22;
        }
        else if (strcmp(options->connexion_type, "telnet") == 0) {
            options->port = 23;
        }
    }

    if (is_empty(options->remote_config) && is_empty(options->remote_server)) {
        options->all=false;
    }

    if (is_empty(options->remote_config) && is_empty(options->remote_server)) {
        if (!is_empty(options->username) || !is_empty(options->password) || !is_empty(options->connexion_type) || options->port != 0) {
            fprintf(stderr, "Erreur: Les options de connexion (-u, -p, -t, -P) ne sont valides qu'avec -s, -l ou -c.\n");
            return -1;
        }
    }

    if (is_empty(options->remote_config)) {
        DIR *d = opendir(".");
        if (d != NULL) { 
            for (struct dirent *element; (element=readdir(d))!=NULL; ) {
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
        fprintf(stderr, "Le fichier %s n'a pas pu être ouvert\n", options->remote_config);
        return EXIT_FAILURE;
    }

    if (f != NULL) {
        fichier tab[MAX_LENGTH];
        int i = 0;
        
        while (fscanf(f, "%49[^;];%49[^;];%d;%49[^;];%49[^;];%4[^;\n]\n",
                        tab[i].name_server,tab[i].adress_server,&tab[i].port,
                        tab[i].username,tab[i].password,tab[i].type_co) == 6){ 
            i++;
        }
        fclose(f); 
    }

    
    
    struct stat fichier;
    if (!is_empty(options->remote_config)) {
        if (stat(options->remote_config, &fichier) == -1) {
             fprintf(stderr, "Erreur: Impossible d'acceder au fichier de configuration '%s'. %s\n", 
                    options->remote_config, strerror(errno));
            return -1;
        }
        if ((fichier.st_mode & 0777) != 0600) {
            fprintf(stderr, "Erreur: Le fichier de configuration '%s' doit avoir les permissions 600 ( rw-------)\n", options->remote_config);
            return -1;
        }
    }

    
    return 0;

}
