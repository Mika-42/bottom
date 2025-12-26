#include "processus.h"
#include "stat_parser.h"
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 

constexpr size_t buf_max_size = 4096;

bool str_is_numeric(const char *str) {
   	
	if (!str || *str == '\0') {
		return false;
	}

	if (*str == '-') {
		++str;
	}

	while (*str) {
		if (!isdigit((unsigned char)(*str))) {
			return false;
		}
		++str;
	}

	return true;
}

bool proc_is_valid_pid(const char *pid) {
	return str_is_numeric(pid) && *pid != '0';
}

FILE *proc_file_open(const pid_t pid, const char *file) {
	
	if (!file) {
		return nullptr;
	}

	char path[PROC_PATH_SIZE];

    snprintf(path, sizeof(path), "/proc/%d/%s", pid, file);

    return fopen(path, "r");
}

error_code_t proc_get_user(processus_t *proc) {
    
	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	FILE *f = proc_file_open(proc->pid, "status");

    if (!f) {
		return OPEN_FILE_FAILED;
	}

	char line[buf_max_size]; 
	const size_t n = fread(line, 1, sizeof(line) - 1, f);

    fclose(f);  
    	
	if (n == 0) return UID_NOT_FOUND;
	line[n] = '\0';

	return stat_user_parser(proc, line);
}


error_code_t proc_get_stat(processus_t *proc) {

	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	FILE *f = proc_file_open(proc->pid, "stat");

	if (!f) {
		return OPEN_FILE_FAILED;
	}

	char line[buf_max_size];

	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);
	return stat_stat_parser(proc, line);
}

error_code_t proc_get_global_stat(long *cpu_total, time_t *boot_time) {

	FILE *f = fopen("/proc/stat", "r");

	if (!f) return OPEN_FILE_FAILED;

	char line[buf_max_size];

	size_t n = fread(line, 1, sizeof(line) - 1, f);
    fclose(f);

    if (n == 0) return READ_FAILED;
	
	line[n] = '\0';
	
	return stat_global_stat_parser(cpu_total, boot_time, line);
}

error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc) {

	if (!proc) {
		return NULLPTR_PARAMETER_ERROR;
	}

	proc->pid = pid;

	proc_get_exe(proc);

	error_code_t err = proc_get_stat(proc);
	
	if (err != SUCCESS) {
		return err;
	}

	err = proc_get_cmdline(proc);

	if (err != SUCCESS) {
		return err;
	}

	err = proc_get_env(proc);
	
	return proc_get_user(proc);
}

void proc_get_exe(processus_t *proc) {
	char path[64];
	snprintf(path, sizeof(path), "/proc/%d/exe", proc->pid);
	ssize_t len = readlink(path, proc->executable, PROC_PATH_SIZE - 1);
	if (len < 0) {
		len = 0;
	}
	proc->executable[len] = '\0';
}

error_code_t proc_read_null_separated(pid_t pid, const char *proc_file, char out[][PROC_CMD_LEN]) {
    if (!proc_file) {
        return NULLPTR_PARAMETER_ERROR;
    }

    char path[PROC_PATH_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/%s", pid, proc_file);
    int fd = open(path, O_RDONLY);

    if (fd < 0) {
        return OPEN_FILE_FAILED;
    }

    char buffer[PROC_CMD_COUNT * PROC_CMD_LEN];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (n <= 0) {
        return READ_FAILED;
	}

    buffer[n] = '\0';
    size_t count = 0;
    char *cur = buffer;

    while (cur < buffer + n && count < PROC_CMD_COUNT) {
        if (*cur == '\0') {
            ++cur;
            continue;
        }

        strncpy(out[count], cur, PROC_CMD_LEN - 1);
        out[count][PROC_CMD_LEN - 1] = '\0';
        ++count;
        cur += strlen(cur) + 1;
    }

    if (count < PROC_CMD_COUNT) {
        out[count][0] = '\0';
	}
    return SUCCESS;
}

error_code_t proc_get_cmdline(processus_t *p) {
	return proc_read_null_separated(p->pid, "cmdline", p->cmdline);
}

error_code_t proc_get_env(processus_t *p) {
	return proc_read_null_separated(p->pid, "environ", p->env);
}

