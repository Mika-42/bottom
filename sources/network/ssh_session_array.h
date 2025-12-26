#ifndef SSH_SESSION_ARRAY_H
#define SSH_SESSION_ARRAY_H

#include "ssh_connexion.h"

typedef struct ssh_session_array_t {
	ssh_session *data;
	size_t size;
	size_t capacity;
} ssh_session_array_t;

void ssh_array_init(ssh_session_array_t *array);
ssh_session *ssh_array_add(ssh_session_array_t *array, ssh_session session);
void ssh_array_free(ssh_session_array_t *array);

#endif /* SSH_SESSION_ARRAY_H */
