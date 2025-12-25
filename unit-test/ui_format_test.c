#include "ui_format.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

bool format_ram_test(const unsigned long test_count, const unsigned long long rss_bytes,double expected_v, const char* expected_unit) {
	double v;
	const char *unit = ui_format_ram(rss_bytes, &v);
	const bool ret = (v == expected_v) && (strncmp(unit, expected_unit, 3) == 0);

	printf(
			"(%2lu | %s) [%10llu B] => %.2f %s\n", 
			test_count, (ret ? "OK" : "NO"),
			rss_bytes, expected_v, expected_unit
	);

	return ret;
}

bool format_time_test(const unsigned long test_count, unsigned long ticks, const char *expected_str) {

	char buf[32];
	const long hz = sysconf(_SC_CLK_TCK);	
	ui_format_time(ticks / hz, buf, 32);

   	const bool ret = (strcmp(buf, expected_str) == 0);

    printf(
            "(%2lu | %s) [%10lu ticks] => %s\n", 
            test_count, (ret ? "OK" : "NO"),
            ticks, expected_str
    );

	return ret;
}

int main() {
	int i = 0;
	int ret = 0;
	printf("============== ui_utils_test ================\n");

	printf("\n---------- format ram -----------------------\n");
	ret += format_ram_test(++i, 1, 1, "  B");
	ret += format_ram_test(++i, 1'024, 1, "KiB");
	ret += format_ram_test(++i, 1'048'576, 1, "MiB");	
	ret += format_ram_test(++i, 1'073'741'824, 1, "GiB");

	printf("\n---------- format time ----------------------\n");	
	const long hz = sysconf(_SC_CLK_TCK);	
	ret += format_time_test(++i, 1 * hz, "00:00:00:01");
	ret += format_time_test(++i, 10 * hz, "00:00:00:10");
	ret += format_time_test(++i, 60 * hz, "00:00:01:00");
	ret += format_time_test(++i, 10 * 60 * hz, "00:00:10:00");
	ret += format_time_test(++i, 60 * 60 * hz, "00:01:00:00");
	ret += format_time_test(++i, 10 * 60 * 60 * hz, "00:10:00:00");
	ret += format_time_test(++i, 24 * 60 * 60 * hz, "01:00:00:00");
	ret += format_time_test(++i, 10 * 24 * 60 * 60 * hz, "10:00:00:00");
	printf("\n %d / %d tests passed. (%3.1f%%)\n\n", ret, i, 100.0f * (float)ret/i);
}
