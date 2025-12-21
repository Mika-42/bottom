#ifndef OPTION_H
#define OPTION_H

#include <getopt.h>
#include <stdio.h>

constexpr size_t field_size = 256;
typedef enum connexion_type_t {
	SSH, TELNET
} connexion_type_t;

typedef struct remote_server_t {
    char server_name[field_size];
    char server_addr[field_size];
    char username[field_size];
    char password[field_size];  
    unsigned short port;
	connexion_type_t connexion;
} remote_server_t;

void opt_print_help();

#endif /* OPTION_H */
