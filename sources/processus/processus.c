#include "processus.h"
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <dirent.h>
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

proc_err_t proc_get_name(const pid_t pid, char *name) {
	
	if(!name) return NULLPTR_PARAMETER_ERROR;
	
	FILE *f = proc_file_open(pid, "comm");

	if (!f) return OPEN_FILE_FAILED;

        if (!fgets(name, PROC_NAME_SIZE, f)) {
		fclose(f);
		return READ_FAILED;
	}
	
	const int end_of_str = strcspn(name, "\n");
	name[end_of_str] = '\0';

        fclose(f);

	return SUCCESS;
}

proc_err_t proc_get_state(const pid_t pid, proc_state_t *state) {
	
	if (!state) return NULLPTR_PARAMETER_ERROR;

	FILE *f = proc_file_open(pid, "stat");

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
		case 'R': *state = RUNNING;	break;
		case 'S': *state = SLEEPING;	break;
		case 'D': *state = DISK_SLEEP;	break;
		case 'T': *state = STOPPED;	break;
		case 't': *state = TRACED;	break;
		case 'Z': *state = ZOMBIE;	break;
		case 'X': *state = DEAD;	break;
		default:  *state = UNKNOW;
	}

    	return SUCCESS;
}

proc_err_t proc_get_user(const pid_t pid, char *username) {
    
	if(!username) return NULLPTR_PARAMETER_ERROR;
	 
	FILE *f = proc_file_open(pid, "status");

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

	strncpy(username,user_info->pw_name,PROC_USERNAME_SIZE);
	username[PROC_USERNAME_SIZE - 1] = '\0';

	return SUCCESS;
}

    
proc_err_t proc_get_rss(const pid_t pid, long* rss) {
	
	if(!rss) return NULLPTR_PARAMETER_ERROR;

	char line[200];

	FILE *f = proc_file_open(pid, "statm");

	if (!f) return OPEN_FILE_FAILED;

	if(fgets(line, sizeof(line), f)) {
		fclose(f);
		return READ_FAILED;
	}

	fclose(f);

	long resident = 0;
	unsigned long _;
	if(sscanf(line,"%lu %ld", &_, &resident) != 2) return MALFORMED_STATUS_LINE;
	
	const long page_size = sysconf(_SC_PAGESIZE);
	*rss = resident * page_size; 
	
	
	return SUCCESS;
}

proc_err_t proc_get_cpu_time(const pid_t pid, unsigned long *utime, unsigned long *stime) {
	FILE *f = proc_file_open(pid, "stat");
	
	if(!f) return OPEN_FILE_FAILED;

	char line[2048];

	if (!fgets(line, sizeof(line), f)) {
    		fclose(f);
    		return READ_FAILED;
	}

	fclose(f);

	sscanf(line, "%*d (%*[^)]) %*c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu %lu", utime, stime);

	return SUCCESS;
}

proc_err_t proc_get_all_infos(const pid_t pid, processus_t *proc) {

	if(!proc) return NULLPTR_PARAMETER_ERROR;

	proc_err_t ret = SUCCESS;
	
	proc->pid = pid;

	ret = proc_get_name(pid, proc->name);
	if(ret != SUCCESS) return ret;

	ret = proc_get_state(pid, &(proc->state));
	if(ret != SUCCESS) return ret;
	
	ret = proc_get_user(pid, proc->user);
	if(ret != SUCCESS) return ret;
	
	ret = proc_get_rss(pid, &(proc->ram_rss));
	if(ret != SUCCESS) return ret;
	
	ret = proc_get_cpu_time(pid, &(proc->utime), &(proc->stime));
	
	return ret;
}

list_t proc_list_get_by_pid(list_t head, const pid_t pid) {
	for(;head != nullptr && head->data.pid != pid; head = head->next);
	return head;
}

proc_err_t proc_list_push_front(list_t *head) {
    	
	if(!head) return NULLPTR_PARAMETER_ERROR;

	list_t new_proc = malloc(sizeof(*new_proc));

	if (!new_proc) return MEMORY_ALLOCATION_FAILED;

	new_proc->next = *head;
	*head = new_proc;

	return SUCCESS; 
}

void proc_list_free(list_t *head) {
	
	if(!head || !(*head)) return;

	while (*head != nullptr) {
		list_t temp = *head;
		*head = (*head)->next;
		free(temp);
	}
	*head = nullptr;
} 

proc_err_t proc_update_list(const char* path, list_t *head) {

	if(!head) return NULLPTR_PARAMETER_ERROR;

	DIR *rep_proc = opendir(path);
	if (!rep_proc) return OPEN_FILE_FAILED;

	struct dirent *ent = nullptr;
    
	while ((ent = readdir(rep_proc))) {
		
		if (!proc_is_valid_pid(ent->d_name)) {
			continue;
		}

		const pid_t pid = atoi(ent->d_name);
		
		list_t l = proc_list_get_by_pid(*head, pid);

		if(!l) {
			proc_list_push_front(head);
			l = *head;
		}

		proc_get_all_infos(pid, &(l->data));
       	
    	}
	
	closedir(rep_proc);

	return SUCCESS;
}
