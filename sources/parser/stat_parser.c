#include "stat_parser.h"
#include <string.h> 
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

error_code_t stat_stat_parser(processus_t *proc, char *line) {
	
	if(!proc || !line) return NULLPTR_PARAMETER_ERROR;

	char *lparen = strchr(line, '(');
	char *rparen = strrchr(line, ')');

	if (!lparen || !rparen || rparen <= lparen) {
		return PARSING_FAILED;
	}

	size_t namelen = rparen - lparen - 1;

	if (namelen >= sizeof(proc->name)) {
		namelen = sizeof(proc->name) - 1;
	}

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

	if (ret != 6) {
		return MALFORMED_STATUS_LINE;
	}

	const unsigned long long page_size = sysconf(_SC_PAGESIZE);
	proc->ram = (unsigned long long)resident * page_size;

	return SUCCESS;
}

error_code_t stat_global_stat_parser(long *cpu_total, time_t *boot_time, char *line) {
	
	if(!cpu_total || !boot_time || !line) return NULLPTR_PARAMETER_ERROR;

	const char *cpu_line = strstr(line, "cpu ");
    if (!cpu_line) return CPU_NOT_FOUND;

	long user, nice, system, idle, iowait, irq, softirq;

	const int n = sscanf(
			cpu_line,
			"cpu  %ld %ld %ld %ld %ld %ld %ld", 
			&user, &nice, &system, &idle, &iowait, &irq, &softirq
	);

	if (n < 7) return MALFORMED_STATUS_LINE;

	*cpu_total = user + nice + system + idle + iowait + irq + softirq;

	const char *btime_line = strstr(line, "btime ");
    if (!btime_line) return BTIME_NOT_FOUND;

	long bt;
	if (sscanf(btime_line, "btime %ld", &bt) != 1) {
		return MALFORMED_STATUS_LINE;
	}

	*boot_time = (time_t)bt;

	return SUCCESS;
}

error_code_t stat_user_parser(processus_t *proc, char *line) {
	
	if(!proc || !line) return NULLPTR_PARAMETER_ERROR;
	
	char *p = strstr(line, "Uid:");
    if (!p) return UID_NOT_FOUND;
	
	uid_t uid = -1;
     if (sscanf(p, "Uid:%u", &uid) != 1) return MALFORMED_STATUS_LINE;

	struct passwd *user_info = getpwuid(uid);  

	if (!user_info) {
		return USER_NOT_FOUND;
	}

	strncpy(proc->user, user_info->pw_name, PROC_USERNAME_SIZE);
	proc->user[PROC_USERNAME_SIZE - 1] = '\0';

	return SUCCESS;
}
