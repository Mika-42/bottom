#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *arg[])
{	
	program_state_t *state = command_parse_options(argc, argv);
	if (state == NULL) {
	   // ya une erreur 
	   return EXIT_FAILURE
 	}
 	if (state->help) {
 	//si ya loption help dmd (à rajouter laide)
 	manager_free_state(state);
 	return EXIT_SUCCESS;
 	
	printf("Hello World!\n");
	return 0;
}		
