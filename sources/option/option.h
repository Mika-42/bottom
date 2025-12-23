#ifndef OPTION_H
#define OPTION_H

#include <getopt.h>
#include <stdio.h>
#include "error.h"
#include "config_file_parser.h"


typedef struct config_file_t config_file_t;

void opt_print_help();

//error_code_t opt_remote_cfg(config_file_t *file, char *opt_path);

#endif /* OPTION_H */
