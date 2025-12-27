#ifndef STAT_PARSER_H
#define STAT_PARSER_H

#include "error.h"
#include "processus.h"

error_code_t stat_stat_parser(processus_t *proc, char *line);
error_code_t stat_global_stat_parser(long *cpu_total, time_t *boot_time, char *line);
error_code_t stat_user_parser(processus_t *proc, char *line);
error_code_t stat_null_separated_parser(const char *buffer, size_t size, char out[][PROC_CMD_LEN]);

#endif /* STAT_PARSER_H */
