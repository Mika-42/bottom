#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H

#include "config_file.h"
#include "option.h"

typedef struct flag_t {
	bool exec_local : 1;
	bool config : 1;
	bool server : 1;
	bool has_opt : 1;
	bool dry_run : 1;
} flag_t;

error_code_t command_run(int argc, char *argv[], flag_t *flag, ssh_session_array_t *sessions, config_file_t *cfg_file);

#endif 
