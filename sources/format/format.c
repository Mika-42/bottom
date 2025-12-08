#include "format.h"
#include <stdio.h>
const char* format_state(const char state) {
	switch(state) {
		case 'R': return "RUNNING";
		case 'S': return "SLEEPING";
		case 'D': return "DISK_SLEEP";
		case 'Z': return "ZOMBIE";
		case 'T': return "STOPPED";
		case 't': return "TRACED";
		case 'W': return "WAKING";
		case 'K': return "WAKEKILL";
		case 'P': return "PARKED";
		case 'X': return "DEAD";
		case 'x': return "DEAD";
		case 'I': return "IDLE";
		default: return "UNKNOW";
	}
}

const char* format_ram(const unsigned long long rss_bytes, double *value) {
	const char *units[] = {"  B", "KiB", "MiB", "GiB", "TiB"};
	*value = (double)rss_bytes;
	int i = 0;

	while (*value >= 1024.0 && i < 4) {
		*value /= 1024.0;
		i++;
	}

	return units[i];
}

void format_time(const unsigned long ticks, char *buf, const size_t bufsize) {
    const long ticks_per_sec = sysconf(_SC_CLK_TCK);
    const unsigned long total_s = ticks / ticks_per_sec;

    const unsigned long s = total_s % 60;
    const unsigned long total_m = total_s / 60;
    const unsigned long m = total_m % 60;
    const unsigned long h = total_m / 60;

    snprintf(buf, bufsize, "%02lu:%02lu:%02lu", h, m, s);
}

