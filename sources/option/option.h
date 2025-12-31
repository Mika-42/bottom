#ifndef OPTION_H
#define OPTION_H

#include <stdio.h>
#include "error.h"
#include "config_file.h"
#include "ssh_connexion.h"
#include "ssh_session_array.h"

void opt_print_help();
[[nodiscard]] error_code_t opt_ask_user(const char *field_name, char *dest);
[[nodiscard]] error_code_t opt_dry_run(const config_file_t *file);
[[nodiscard]] error_code_t opt_connect(const config_file_t *file, ssh_session_array_t *array);

#endif /* OPTION_H */
