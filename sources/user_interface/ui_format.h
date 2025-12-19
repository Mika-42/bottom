#ifndef UI_FORMAT_H
#define UI_FORMAT_H

#include <stddef.h>

[[nodiscard]] const char	*ui_format_state(const char state);
[[nodiscard]] const char	*ui_format_ram(const unsigned long long rss_bytes, double *value);
void		ui_format_time(unsigned long ticks, char *buf, const size_t bufsize);

#endif //UI_FORMAT_H
