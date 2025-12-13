#include "ui_format.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

const char* ui_format_state(const char state) {
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

const char* ui_format_ram(const unsigned long long rss_bytes, double *value) {
	const char *units[] = {"  B", "KiB", "MiB", "GiB", "TiB"};
	*value = (double)rss_bytes;
	int i = 0;

	while (*value >= 1024.0 && i < 4) {
		*value /= 1024.0;
		i++;
	}

	return units[i];
}

void ui_format_time(const unsigned long ticks, char *buf, const size_t bufsize) {

	const long tps = sysconf(_SC_CLK_TCK);
	if (tps <= 0) {
		snprintf(buf, bufsize, "??:??:??:??");
		return;
	}

	uint64_t total_s = ticks / (uint64_t)tps;

	uint64_t s = total_s % 60;
	uint64_t total_m = total_s / 60;

	uint64_t m = total_m % 60;
	uint64_t total_h = total_m / 60;

	uint64_t h = total_h % 24;
	uint64_t d = total_h / 24;

	snprintf(buf, bufsize, "%02llu:%02llu:%02llu:%02llu",
			(unsigned long long)d,
			(unsigned long long)h,
			(unsigned long long)m,
			(unsigned long long)s);
}
