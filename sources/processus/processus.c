#include "processus.h"
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 

bool str_is_numeric(const char *str) {
   	
	if (!str || *str == '\0') return false;

	if (*str == '-') ++str;

	while (*str) {
		if (!isdigit((unsigned char)(*str))) return false;
		
		++str;
	}

	return true;
}

bool proc_is_valid_pid(const char *pid) {
	return str_is_numeric(pid) && *pid != '0';
}

FILE *proc_file_open(const pid_t pid, const char *file) {
	
	if (!file) return nullptr;

	char path[PROC_PATH_SIZE];

        snprintf(path, sizeof(path), "/proc/%d/%s", pid, file);

        return fopen(path, "r");
}

error_code_t proc_get_user(processus_t *proc) {
    
	if (!proc) return NULLPTR_PARAMETER_ERROR;
	 
	FILE *f = proc_file_open(proc->pid, "status");

    	if (!f) return OPEN_FILE_FAILED;

	char line[512]; 
	uid_t uid = -1;
        
	while (fgets(line, sizeof(line), f)) {
            
		if (strncmp(line, "Uid:", 4) != 0) continue;

		const int ret = sscanf(line, "%*s %u", &uid);
               	if (ret != 1) {
					fclose(f);
					return MALFORMED_STATUS_LINE;
				}
                break;	
        }

        fclose(f);  
    	
	if (uid == (uid_t)-1) return UID_NOT_FOUND;

	struct passwd *user_info = getpwuid(uid);  

	if (!user_info) return USER_NOT_FOUND;

	strncpy(proc->user,user_info->pw_name,PROC_USERNAME_SIZE);
	proc->user[PROC_USERNAME_SIZE - 1] = '\0';

	return SUCCESS;
}

error_code_t proc_get_stat(processus_t *proc) {

	if (!proc) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(proc->pid, "stat");

	if (!f) return OPEN_FILE_FAILED;

	char line[4096];

	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);
	
	char *lparen = strchr(line, '(');
	char *rparen = strrchr(line, ')');
	
	if (!lparen || !rparen || rparen <= lparen) return PARSING_FAILED; 

	size_t namelen = rparen - lparen - 1;
	
	if (namelen >= sizeof(proc->name)) namelen = sizeof(proc->name) - 1;
	
	strncpy(proc->name, lparen + 1, namelen);
	proc->name[namelen] = '\0';
	
	long resident = 0;
	int ret = sscanf(rparen + 2, 
		"%c "
		"%d "
		"%*s %*s %*s %*s %*s %*s %*s %*s %*s "
		"%lu %lu "
		"%*s %*s %*s %*s %*s %*s "
		"%lu "
		"%*s "
		"%ld",  
		&proc->state,
		&proc->ppid,
		&proc->utime, 
		&proc->stime, 
		&proc->start_time, 
		&resident);
	
	if (ret != 6) return MALFORMED_STATUS_LINE;

	const unsigned long long page_size = sysconf(_SC_PAGESIZE);
	proc->ram = (unsigned long long)resident * page_size;

	return SUCCESS;
}

error_code_t proc_get_global_stat(long *cpu_total, time_t *boot_time) {

	FILE *f = fopen("/proc/stat", "r");

	if (!f) return OPEN_FILE_FAILED;


	char line[512];

	if (!fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	long user, nice, system, idle, iowait, irq, softirq;

	int n = sscanf(line, "cpu  %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
	if (n < 7) return MALFORMED_STATUS_LINE;

	*cpu_total = user + nice + system + idle + iowait + irq + softirq;

	*boot_time = 0;
    	while (fgets(line, sizeof(line), f)) {
        	if (sscanf(line, "btime %ld", boot_time) == 1) break;
    	}

	fclose(f);
	return SUCCESS;
}

error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc) {

	if (!proc) return NULLPTR_PARAMETER_ERROR;

	proc->pid = pid;

	proc_get_exe(proc);

	error_code_t err = proc_get_stat(proc);
	
	if (err != SUCCESS) return err;

	err = proc_get_cmdline(proc);

	if (err != SUCCESS) return err;
	
	err = proc_get_env(proc);
	
	return proc_get_user(proc);
}

void proc_get_exe(processus_t *proc) {
	char path[64];
	snprintf(path, sizeof(path), "/proc/%d/exe", proc->pid);
	ssize_t len = readlink(path, proc->executable, PROC_PATH_SIZE - 1);
	if (len < 0) len = 0;
	proc->executable[len] = '\0';
}

error_code_t proc_read_null_separated(pid_t pid, const char *proc_file, char out[][PROC_CMD_LEN]) {
    if (!proc_file)
        return NULLPTR_PARAMETER_ERROR;

    char path[PROC_PATH_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/%s", pid, proc_file);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return OPEN_FILE_FAILED;

    char buffer[PROC_CMD_COUNT * PROC_CMD_LEN];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (n <= 0)
        return READ_FAILED;

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

    if (count < PROC_CMD_COUNT)
        out[count][0] = '\0';

    return SUCCESS;
}

error_code_t proc_get_cmdline(processus_t* p) {
	return proc_read_null_separated(p->pid, "cmdline", p->cmdline);
}

error_code_t proc_get_env(processus_t* p) {
	return proc_read_null_separated(p->pid, "environ", p->env);
}

