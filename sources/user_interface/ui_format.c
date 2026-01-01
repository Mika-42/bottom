#include "ui_format.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

const char *ui_format_state(const char state) {
    switch (state) {
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

const char *ui_format_ram(const unsigned long long rss_bytes, double *value) {
    
    if (!value) { 
        return nullptr;
    }
    
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
    unsigned long s = ticks % 60;
    unsigned long total_m = ticks / 60;
    unsigned long m = total_m % 60;
    unsigned long total_h = total_m / 60;
    unsigned long h = total_h % 24;
    unsigned long d = total_h / 24;

    snprintf(buf, bufsize, "%02lu:%02lu:%02lu:%02lu", d, h, m, s);
}
