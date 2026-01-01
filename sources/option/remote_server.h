#ifndef REMOTE_SERVER_H
#define REMOTE_SERVER_H

#include <stddef.h>
#include "error.h"

constexpr size_t field_size = 256;
constexpr size_t max_port = 65'535;

typedef enum connexion_type_t {
    SSH, TELNET
} connexion_type_t;

typedef struct remote_server_t {
    char name[field_size];
    char address[field_size];
    char username[field_size];
    char password[field_size];  
    unsigned short port;
    connexion_type_t connexion;
} remote_server_t;

[[nodiscard]] error_code_t srv_str_duplicate(char *dest, char *src);
[[nodiscard]] error_code_t srv_str_to_uint16(unsigned short *dest, char *src);
[[nodiscard]] error_code_t srv_str_to_connexion(connexion_type_t *dest, char *src);
[[nodiscard]] error_code_t srv_str_split_at(char *dest_lhs, char *dest_rhs, char *src);
[[nodiscard]] bool srv_str_is_empty(char *str);
[[nodiscard]] bool srv_is_valid(remote_server_t *server);

#endif /* REMOTE_SERVER_H */
