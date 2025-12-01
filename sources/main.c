#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "command_option.h"
#include "ui.h"

int main(int argc, char *argv[]){	

    options_prog options = {
        .help = false,
        .dry_run = false,
        .remote_config = NULL,
        .connexion_type = NULL,
        .port = 0, 
        .login = NULL,
        .remote_server = NULL,
        .username = NULL,
        .password = NULL,
        .all = false
    };

    int gerer_options = command_run(argc, argv, &options);

    if (gerer_options !=0) {
        fprintf(stderr, "Erreur lors de l'analyse des options. Utilisez -h pour l'aide.\n");
        return EXIT_FAILURE;
    }
    
}

