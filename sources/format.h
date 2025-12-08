#ifndef FORMAT_H
#define FORMAT_H

const char* format_state(const char state);
const char* format_ram(const unsigned long long rss_bytes, double *value);
void format_time(const unsigned long ticks, char *buf, const size_t bufsize);

#endif //FORMAT_H
