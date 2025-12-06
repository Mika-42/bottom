#include "processus.h"
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <stdlib.h>

bool str_is_numeric(const char *str) {
   	
	if(!str || *str == '\0') return false;

	if(*str == '-') ++str;

	while (*str) {
		if(!isdigit((unsigned char)(*str))) {
			return false;
		}
		++str;
	}

	return true;
}

bool proc_is_valid_pid(const char *pid) {
	return str_is_numeric(pid) && (*pid != '-' || *pid != '0');
}

FILE *proc_file_open(const pid_t pid, const char *file) {
	if(!file) return nullptr;

	char path[PROC_PATH_SIZE];

        snprintf(path, sizeof(path), "/proc/%d/%s", pid, file);

        return fopen(path, "r");
}

error_code_t proc_get_name(processus_t *proc) {
	
	if (!proc) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(proc->pid, "comm");

	if (!f) return OPEN_FILE_FAILED;

        if (!fgets(proc->name, PROC_NAME_SIZE, f)) {
		fclose(f);
		return READ_FAILED;
	}
	
	const int end_of_str = strcspn(proc->name, "\n");
	proc->name[end_of_str] = '\0';

        fclose(f);

	return SUCCESS;
}

error_code_t proc_get_state(processus_t *proc) {
	
	if (!proc) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(proc->pid, "stat");

	if (!f) return OPEN_FILE_FAILED; 
        
	char buffer[512];
        
	if (!fgets(buffer, sizeof(buffer), f)) { 
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);

	char temp = '\0';

	/*
	 * %*d		--> ignore the first integer (pid)
	 * (		--> read the '(' character
	 * %*[^)]	--> ignore all character until ')' has been reached
	 * )		--> read the ')' character
	 * %c		--> get the character
	 */

	const int ret = sscanf(buffer, "%*d (%*[^)]) %c", &temp);

	if(ret != 1) return MALFORMED_STATUS_LINE; 
        
	switch(temp) {
		case 'R': proc->state = RUNNING;	break;
		case 'S': proc->state = SLEEPING;	break;
		case 'D': proc->state = DISK_SLEEP;	break;
		case 'Z': proc->state = ZOMBIE;		break;
		case 'T': proc->state = STOPPED;	break;
		case 't': proc->state = TRACED;		break;
		case 'W': proc->state = WAKING;		break;	
		case 'K': proc->state = WAKEKILL;	break;
		case 'P': proc->state = PARKED;		break;  
		case 'X': proc->state = DEAD;		break;
		case 'x': proc->state = DEAD;		break;
		case 'I': proc->state = IDLE;		break;
		default:  proc->state = UNKNOW;
	}

    	return SUCCESS;
}

error_code_t proc_get_user(processus_t *proc) {
    
	if(!proc) return NULLPTR_PARAMETER_ERROR;
	 
	FILE *f = proc_file_open(proc->pid, "status");

    	if (!f) return OPEN_FILE_FAILED;

	char line[512]; 
	uid_t uid = -1;
        
	while (fgets(line, sizeof(line), f)) {
            
		if (strncmp(line, "Uid:", 4) != 0) continue;

		const int ret = sscanf(line, "%*s %u", &uid);
               	if(ret != 1) return MALFORMED_STATUS_LINE;

                break;	
        }

        fclose(f);  
    	
	if(uid == (uid_t)-1) return UID_NOT_FOUND;

	struct passwd *user_info = getpwuid(uid);  

	if (!user_info) return USER_NOT_FOUND;

	strncpy(proc->user,user_info->pw_name,PROC_USERNAME_SIZE);
	proc->user[PROC_USERNAME_SIZE - 1] = '\0';

	return SUCCESS;
}

    
error_code_t proc_get_rss(processus_t *proc) {
	
	if(!proc) return NULLPTR_PARAMETER_ERROR;

	char line[200];

	FILE *f = proc_file_open(proc->pid, "statm");

	if (!f) return OPEN_FILE_FAILED;

	if(!fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);

	long resident = 0;
	unsigned long _;
	if(sscanf(line,"%lu %ld", &_, &resident) != 2) return MALFORMED_STATUS_LINE;
	
	const long page_size = sysconf(_SC_PAGESIZE);
	proc->ram_rss = resident * page_size; 
	
	return SUCCESS;
}

error_code_t proc_get_cpu_time(processus_t *proc) {

	if(!proc) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(proc->pid, "stat");
	
	if(!f) return OPEN_FILE_FAILED;

	char line[2048];

	if (!fgets(line, sizeof(line), f)) {
    		fclose(f);
    		return READ_FAILED;
	}

	fclose(f);

	sscanf(line, "%*d (%*[^)]) %*c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu %lu", &proc->utime, &proc->stime);

	return SUCCESS;
}

error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc) {

	if(!proc) return NULLPTR_PARAMETER_ERROR;
	
	proc->pid = pid;

	typedef error_code_t (*proc_getter_t)(processus_t *proc);
	
	proc_getter_t getters[] = { 
	        proc_get_name, 
		proc_get_state, 
		proc_get_user, 
		proc_get_rss, 
		proc_get_cpu_time,
		nullptr
	};
	
	for (int i = 0; getters[i] != nullptr; ++i) {
		error_code_t err = getters[i](proc);
		if (err != SUCCESS) return err;
	}
	
	return SUCCESS;
}
