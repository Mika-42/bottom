#include <stdio.h>
#include <stdlib.h>
#include "command_option.h"
#include "thread.h"

int main(int argc, char *argv[]){	
	flag_t flag = {
		.exec_local = false,
		.config = false,
		.server = false,
		.has_opt = false,
		.dry_run = false
	};
	return command_run(argc, argv, &flag);
//	thread_args_t args;
//	return (bottom_ui(&args) != SUCCESS) ? EXIT_FAILURE : EXIT_SUCCESS;
}

