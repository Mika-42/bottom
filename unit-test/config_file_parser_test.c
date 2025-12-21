#include "config_file_parser.h"
#include "debug.h"
#include <stdio.h>

bool cfg_load_file_test(/*const unsigned long test_count*/) {
	char path[path_max_size] = "unit-test/.config";
	config_file_t f;
	cfg_init(&f);
	error_code_t err = cfg_load_file(&f, path);

	if(err != SUCCESS) {
		printf("\n\nerr: %s, path:%s\n\n", err_to_str(err), path);
		return false;
	}

	printf("%s:%s:%s:%s:%d:%s\n",f.data[0].server_name, f.data[0].server_addr, f.data[0].username, f.data[0].password, f.data[0].port, f.data[0].connexion == SSH ? "SSH" : "Telnet");

	cfg_free(&f);
	return true;
}

int main() {
	cfg_load_file_test();
   return 0;	
}
