#include "remote_server.h"
#include <string.h>
#include <stdlib.h>

error_code_t srv_str_duplicate(char *dest, char *src) {

	if(!dest || !src) return NULLPTR_PARAMETER_ERROR;

    size_t len = strnlen(src, field_size);

	if(len >= field_size) return BUFFER_TOO_LONG;
	
	memcpy(dest, src, len);
    dest[len] = '\0';

	return SUCCESS;
}

error_code_t srv_str_to_uint16(unsigned short *dest, char *src) {

	if(!dest || !src) return NULLPTR_PARAMETER_ERROR;

	char *endptr;
	long n = strtol(src, &endptr, 10);

	if(*endptr != '\0' || n < 0 || (size_t)n > max_port) return INVALID_ARGUMENT;
	*dest = (unsigned short)n;

	return SUCCESS;
}

error_code_t srv_str_to_connexion(connexion_type_t *dest, char *src) {
	
	if(!dest || !src) return NULLPTR_PARAMETER_ERROR;
	
	if (strcasecmp(src, "ssh") == 0) {
		*dest = SSH;
		return SUCCESS;
	}
    
	if (strcasecmp(src, "telnet") == 0) {
	   *dest = TELNET;
	   return SUCCESS;
	}

    return INVALID_ARGUMENT;
}

error_code_t srv_str_split_at(char *dest_lhs, char *dest_rhs, char *src) {

	if(!dest_lhs || !dest_rhs || !src) return NULLPTR_PARAMETER_ERROR;

	char copy[field_size];	
	error_code_t err = srv_str_duplicate(copy, src);
	if(err != SUCCESS) return err;

	char *separator = strchr(copy, '@');
	if (!separator || separator == copy || *(separator + 1) == '\0')
		return INVALID_ARGUMENT;

	if (strchr(separator + 1, '@'))
		return INVALID_ARGUMENT;

	err = srv_str_duplicate(dest_rhs, separator + 1);
	if(err != SUCCESS) return err;
	
	*separator = '\0';

	return srv_str_duplicate(dest_lhs, copy);
}

bool srv_str_is_empty(char *str) {
	return *str == '\0';
}
