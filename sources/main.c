#include <stdio.h>
#include <stdlib.h>
#include "command_option.h"
#include "thread.h"

int main(int argc, char *argv[]){	

	   options_prog options = {
	   .help = false,
	   .dry_run = false,
	   .port = 0, 
	   .all = false
	   };

	   int gerer_options = command_run(argc, argv, &options);

	   if (gerer_options !=0) {
	   fprintf(stderr, "Erreur lors de l'analyse des options. Utilisez -h pour l'aide.\n");
	   return EXIT_FAILURE;
	   }

	   return 0;
//	thread_args_t args;
//	return (bottom_ui(&args) != SUCCESS) ? EXIT_FAILURE : EXIT_SUCCESS;
}

