#include "ssh_processus.h"
#include "stat_parser.h"

#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 

constexpr size_t max_proc = 8192;
error_code_t ssh_get_user(processus_t *proc, ssh_session session) {
    
	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	char path[64];
	char line[buf_max_size];
	snprintf(path, sizeof(path), "%d/status", proc->pid);
	
	if(ssh_get_file(session, line, buf_max_size, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	return stat_user_parser(proc, line);
}


error_code_t ssh_get_stat(processus_t *proc, ssh_session session) {

	char path[64];
	char line[buf_max_size];
	snprintf(path, sizeof(path), "%d/stat", proc->pid);
	
	if(ssh_get_file(session, line, buf_max_size, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	return stat_stat_parser(proc, line);
}

error_code_t ssh_get_global_stat(long *cpu_total, time_t *boot_time, ssh_session session) {

	char line[buf_max_size];
	
	if(ssh_get_file(session, line, buf_max_size, "stat") != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}
	return stat_global_stat_parser(cpu_total, boot_time, line);
}

error_code_t ssh_get_all_infos(const pid_t pid, processus_t *proc, ssh_session session
) {

	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	proc->pid = pid;

	error_code_t err = ssh_get_stat(proc, session);
	
	if (err != SUCCESS) {
		return err;
	}

	return ssh_get_user(proc, session);
}

error_code_t ssh_get_cmdline(processus_t *proc, ssh_session session) {
	
	char path[64];
	char line[buf_max_size];
	snprintf(path, sizeof(path), "%d/cmdline", proc->pid);
	
	if(ssh_get_file(session, line, buf_max_size, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}
	memset(proc->cmdline, 0, sizeof(proc->cmdline));

	return stat_null_separated_parser(line, sizeof(proc->cmdline), proc->cmdline);
}

error_code_t ssh_get_env(processus_t *proc, ssh_session session) {
	char path[64];
	char line[buf_max_size];
	snprintf(path, sizeof(path), "%d/environ", proc->pid);
	
	if(ssh_get_file(session, line, buf_max_size, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}
	memset(proc->env, 0, sizeof(proc->env));

	return stat_null_separated_parser(line, sizeof(proc->env), proc->env);
}

error_code_t ssh_array_update(processus_array_t *array, ssh_session session) {

	if (!array) {
		return NULLPTR_PARAMETER_ERROR;
	}
	
	pid_t pid_list[max_proc];
	size_t count;
	
   error_code_t err	= ssh_get_pid_list(session, pid_list, max_proc, &count);

	if (err != SUCCESS) {
		return err;
	}

	const size_t limit = count < max_proc ? count : max_proc;

	proc_array_reset(array);

	for(size_t i=0; i<limit; ++i) {

		processus_t *proc = proc_array_emplace_back(array);

		if (!proc) {
			return MEMORY_ALLOCATION_FAILED;
		}

		if (ssh_get_all_infos(pid_list[i], proc, session) != SUCCESS) {
			processus_t *last = proc_array_get_last(array);

			// on écrase le proc mort avec le dernier proc de la liste
			if (proc != last) {
				*proc = *last;
			}
			// on retire le dernier proc
			array->size--;
		}
	}

	return ssh_get_global_stat(&array->cpu_tick, &array->boot_time, session);
}

