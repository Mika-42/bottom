#include "ssh_processus.h"
#include "stat_parser.h"
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 

constexpr size_t buf_max_size = 4096;

error_code_t ssh_get_user(processus_t *proc, ssh_session session) {
    
	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	char path[64];
	char line[buf_max_size];
	snprintf(path, sizeof(path), "%d/status", proc->pid);
	
	if(ssh_get_file(session, line, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	error_code_t err = stat_user_parser(proc, line);
	free(line);

	return err;
}


error_code_t ssh_get_stat(processus_t *proc, ssh_session session) {

	char path[64];
	char *line;
	snprintf(path, sizeof(path), "%d/stat", proc->pid);
	
	if(ssh_get_file(session, &line, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	error_code_t err = stat_stat_parser(proc, line);
	free(line);
	return err;
}

error_code_t ssh_get_global_stat(long *cpu_total, time_t *boot_time, ssh_session session) {

	char *line;
	
	if(ssh_get_file(session, &line, "stat") != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}
	error_code_t err = stat_global_stat_parser(cpu_total, boot_time, line);
	free(line);
	return err;
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
	
	if(ssh_get_file(session, &line, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	error_code_t err = stat_null_separated_parser(line, sizeof(proc->cmdline), proc->cmdline);
}

error_code_t ssh_get_env(processus_t *proc, ssh_session) {
	char path[64];
	char *line;
	snprintf(path, sizeof(path), "%d/environ", proc->pid);
	
	if(ssh_get_file(session, &line, path) != SSH_OK) {
		return SSH_GET_FILE_FAILED;
	}

	return stat_null_separated_parser(buffer, sizeof(p->env), p->env);
}

error_code_t ssh_array_update(processus_array_t *array, ssh_session session) {

	if (!array) {
		return NULLPTR_PARAMETER_ERROR;
	}

	/*
	DIR *rep_proc = opendir("/proc");
	if (!rep_proc) {
		return OPEN_FILE_FAILED;
	}

	struct dirent *ent = nullptr;
	*/
	proc_array_reset(array);

	while (/*(ent = readdir(rep_proc)) != nullptr*/) {

		char *end = nullptr;
		const pid_t pid = strtol(ent->d_name, &end, 10);
		if (*end != '\0' || pid <= 0) { 
			continue;
		}

		processus_t *proc = proc_array_emplace_back(array);

		if (!proc) {
			closedir(rep_proc);
			return MEMORY_ALLOCATION_FAILED;
		}

		if (ssh_get_all_infos(pid, proc, session) != SUCCESS) {
			processus_t *last = proc_array_get_last(array);

			// on écrase le proc mort avec le dernier proc de la liste
			if (proc != last) {
				*proc = *last;
			}
			// on retire le dernier proc
			array->size--;
		}
	}

//	closedir(rep_proc);

	return ssh_get_global_stat(&array->cpu_tick, &array->boot_time);
}

