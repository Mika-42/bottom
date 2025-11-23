#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "command_option.h"

int main(int argc, char *argv[])
{	
    program_state_t *state = command_parse_options(argc, argv);
    if (state == NULL) {
        // ya une erreur 
        return EXIT_FAILURE;
    }

    if (state->help) {
        // si l'option --help est demandée
        print_help();
        manager_free_state(state);
        return EXIT_SUCCESS;
    }


    // libération de la mémoire
    manager_free_state(state);

    return EXIT_SUCCESS;
}

