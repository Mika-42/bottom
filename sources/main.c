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

		return command_run(argc, argv, &options);
//	thread_args_t args;
//	return (bottom_ui(&args) != SUCCESS) ? EXIT_FAILURE : EXIT_SUCCESS;
}

