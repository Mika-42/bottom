#include "ssh_processus.h"
#include "stat_parser.h"

#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 

error_code_t ssh_array_update(processus_array_t *array, ssh_session session) {

	if (!array || !session) {
		return NULLPTR_PARAMETER_ERROR;
	}
	
	const char *cmd = "ps -e -o pid,ppid,stat,rss,user:32,etimes,pcpu,comm:256 --no-headers";
	constexpr size_t buffer_size = 524'288; // 512 KB
	char *buffer = malloc(buffer_size);
	if (!buffer) return MEMORY_ALLOCATION_FAILED;
	memset(buffer, 0, buffer_size);
	
	error_code_t err = ssh_cmd_exec(session, buffer, buffer_size, cmd);

	if(err != SUCCESS) {
		return err;
	}

	buffer[buffer_size - 1] = '\0';
	proc_array_reset(array);
	
	char *line = strtok(buffer, "\n");	
	while(line) {
		processus_t *p = proc_array_emplace_back(array);
		char tmp_state[8];
		char tmp_user[64];
		int n = sscanf(line, " %d %d %7s %llu %63s %lu %lf %255[^\n]", 
				&p->pid,
				&p->ppid,
				tmp_state,
				&p->ram,
				tmp_user,
				&p->start_time,
				&p->cpu_usage,
				p->name
		);
		
		p->state = tmp_state[0];
		strncpy(p->user, tmp_user, sizeof(p->user));
		p->user[sizeof(p->user)-1] = '\0';

		if(strcmp(p->name, "ps") == 0) {	
			--array->size;
		}

		if(n < 8) {
			--array->size;
			return MALFORMED_STATUS_LINE;
		}

		line = strtok(nullptr, "\n");
	}

	free(buffer);
	return SUCCESS;
}
